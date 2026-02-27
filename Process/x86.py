#! /usr/bin/env python3

import sys
import time
import random
from optparse import OptionParser

#
# HELPER
#
def dospace(howmuch):
    for i in range(howmuch):
        print('%24s' % ' ', end=' ')

# useful instead of assert
def zassert(cond, str):
    if cond == False:
        print('ABORT::', str)
        exit(1)
    return

class cpu:
    #
    # INIT: how much memory?
    #
    def __init__(self, memory, memtrace, regtrace, cctrace, compute, verbose):
        #
        # CONSTANTS
        #
        
        # conditions
        self.COND_GT        = 0
        self.COND_GTE       = 1
        self.COND_LT        = 2
        self.COND_LTE       = 3
        self.COND_EQ        = 4
        self.COND_NEQ       = 5

        # registers in system
        self.REG_ZERO       = 0
        self.REG_AX         = 1
        self.REG_BX         = 2
        self.REG_CX         = 3
        self.REG_DX         = 4
        self.REG_SP         = 5
        self.REG_BP         = 6

        # system memory: in KB
        self.max_memory     = memory * 1024

        # which memory addrs and registers to trace?
        self.memtrace       = memtrace
        self.regtrace       = regtrace
        self.cctrace        = cctrace
        self.compute        = compute
        self.verbose        = verbose

        self.PC             = 0
        self.registers      = {}
        self.conditions     = {}
        self.labels         = {}
        self.vars           = {}
        self.memory         = {}
        self.pmemory        = {}  # for printable version of what's in memory (instructions)

        self.condlist       = [self.COND_GTE, self.COND_GT, self.COND_LTE, self.COND_LT, self.COND_NEQ, self.COND_EQ]
        self.regnums        = [self.REG_ZERO, self.REG_AX,  self.REG_BX,   self.REG_CX,  self.REG_DX,   self.REG_SP,  self.REG_BP]

        self.regnames         = {}
        self.regnames['zero'] = self.REG_ZERO # hidden zero-valued register
        self.regnames['ax']   = self.REG_AX
        self.regnames['bx']   = self.REG_BX
        self.regnames['cx']   = self.REG_CX
        self.regnames['dx']   = self.REG_DX
        self.regnames['sp']   = self.REG_SP
        self.regnames['bp']   = self.REG_BP

        tmplist = []
        for r in self.regtrace:
            zassert(r in self.regnames, 'Register %s cannot be traced because it does not exist' % r)
            tmplist.append(self.regnames[r])
        self.regtrace = tmplist

        self.init_memory()
        self.init_registers()
        self.init_condition_codes()

    #
    # BEFORE MACHINE RUNS
    #
    def init_condition_codes(self):
        for c in self.condlist:
            self.conditions[c] = False

    def init_memory(self):
        for i in range(self.max_memory):
            self.memory[i] = 0

    def init_registers(self):
        for i in self.regnums:
            self.registers[i] = 0

    def dump_memory(self):
        print('MEMORY DUMP')
        for i in range(self.max_memory):
            if i not in self.pmemory and i in self.memory and self.memory[i] != 0:
                print('  m[%d]' % i, self.memory[i])

    #
    # INFORMING ABOUT THE HARDWARE
    #
    def get_regnum(self, name):
        assert(name in self.regnames)
        return self.regnames[name]

    def get_regname(self, num):
        assert(num in self.regnums)
        for rname in self.regnames:
            if self.regnames[rname] == num:
                return rname
        return ''
    
    def get_regnums(self):
        return self.regnums

    def get_condlist(self):
        return self.condlist

    def get_reg(self, reg):
        assert(reg in self.regnums)
        return self.registers[reg]

    def get_cond(self, cond):
        assert(cond in self.condlist)
        return self.conditions[cond]

    def get_pc(self):
        return self.PC
        
    def set_reg(self, reg, value):
        assert(reg in self.regnums)
        self.registers[reg] = value

    def set_cond(self, cond, value):
        assert(cond in self.condlist)
        self.conditions[cond] = value

    def set_pc(self, pc):
        self.PC = pc
        
    #
    # INSTRUCTIONS
    #
    def halt(self):
        return -1

    def iyield(self):
        return -2

    def nop(self):
        return 0

    def rdump(self):
        print('REGISTERS::', end=' ')
        print('ax:', self.registers[self.REG_AX], end=' ')
        print('bx:', self.registers[self.REG_BX], end=' ')
        print('cx:', self.registers[self.REG_CX], end=' ')
        print('dx:', self.registers[self.REG_DX])

    def mdump(self, index):
        print('m[%d] ' % index, self.memory[index])

    def move_i_to_r(self, src, dst):
        self.registers[dst] = src
        return 0

    # memory: value, register, register
    def move_i_to_m(self, src, value, reg1, reg2):
        tmp = value + self.registers[reg1] + self.registers[reg2]
        self.memory[tmp] = src
        return 0

    def move_m_to_r(self, value, reg1, reg2, dst):
        tmp = value + self.registers[reg1] + self.registers[reg2]
        self.registers[dst] = self.memory[tmp] 

    def move_r_to_m(self, src, value, reg1, reg2):
        tmp = value + self.registers[reg1] + self.registers[reg2]
        self.memory[tmp] = self.registers[src]
        return 0

    def move_r_to_r(self, src, dst):
        self.registers[dst] = self.registers[src]
        return 0

    def add_i_to_r(self, src, dst):
        self.registers[dst] += src
        return 0

    def add_r_to_r(self, src, dst):
        self.registers[dst] += self.registers[src]
        return 0

    def sub_i_to_r(self, src, dst):
        self.registers[dst] -= src
        return 0

    def sub_r_to_r(self, src, dst):
        self.registers[dst] -= self.registers[src]
        return 0


    #
    # SUPPORT FOR LOCKS
    #
    def atomic_exchange(self, src, value, reg1, reg2):
        tmp                 = value + self.registers[reg1] + self.registers[reg2]
        old                 = self.memory[tmp]
        self.memory[tmp]    = self.registers[src]
        self.registers[src] = old
        return 0

    def fetchadd(self, src, value, reg1, reg2):
        tmp                 = value + self.registers[reg1] + self.registers[reg2]
        old                 = self.memory[tmp]
        self.memory[tmp]    = self.memory[tmp] + self.registers[src] 
        self.registers[src] = old

    #
    # TEST for conditions
    #
    def test_all(self, src, dst):
        self.init_condition_codes()
        if dst > src:
            self.conditions[self.COND_GT]  = True
        if dst >= src:
            self.conditions[self.COND_GTE] = True
        if dst < src:
            self.conditions[self.COND_LT]  = True
        if dst <= src:
            self.conditions[self.COND_LTE] = True
        if dst == src:
            self.conditions[self.COND_EQ]  = True
        if dst != src:
            self.conditions[self.COND_NEQ] = True
        return 0

    def test_i_r(self, src, dst):
        self.init_condition_codes()
        return self.test_all(src, self.registers[dst])

    def test_r_i(self, src, dst):
        self.init_condition_codes()
        return self.test_all(self.registers[src], dst)

    def test_r_r(self, src, dst):
        self.init_condition_codes()
        return self.test_all(self.registers[src], self.registers[dst])

    #
    # JUMPS
    #
    def jump(self, targ):
        self.PC = targ  
        return 0
    
    def jump_notequal(self, targ):
        if self.conditions[self.COND_NEQ] == True:
            self.PC = targ
        return 0

    def jump_equal(self, targ):
        if self.conditions[self.COND_EQ] == True:
            self.PC = targ
        return 0

    def jump_lessthan(self, targ):
        if self.conditions[self.COND_LT] == True:
            self.PC = targ
        return 0

    def jump_lessthanorequal(self, targ):
        if self.conditions[self.COND_LTE] == True:
            self.PC = targ
        return 0

    def jump_greaterthan(self, targ):
        if self.conditions[self.COND_GT] == True:
            self.PC = targ
        return 0

    def jump_greaterthanorequal(self, targ):
        if self.conditions[self.COND_GTE] == True:
            self.PC = targ
        return 0

    #
    # CALL and RETURN
    #
    def call(self, targ):
        self.registers[self.REG_SP] -= 4
        self.memory[self.registers[self.REG_SP]] = self.PC 
        self.PC = targ

    def ret(self):
        self.PC = self.memory[self.registers[self.REG_SP]]
        self.registers[self.REG_SP] += 4

    #
    # STACK and related
    #
    def push_r(self, reg):
        self.registers[self.REG_SP] -= 4
        self.memory[self.registers[self.REG_SP]] = self.registers[reg]
        return 0

    def push_m(self, value, reg1, reg2):
        self.registers[self.REG_SP] -= 4
        tmp = value + self.registers[reg1] + self.registers[reg2]
        self.memory[self.registers[self.REG_SP]] = tmp
        return 0

    def pop(self):
        self.registers[self.REG_SP] += 4

    def pop_r(self, dst):
        self.registers[dst] = self.registers[self.REG_SP]
        self.registers[self.REG_SP] += 4

    #
    # HELPER func for getarg
    #
    def register_translate(self, r):
        if r in self.regnames:
            return self.regnames[r]
        zassert(False, 'Register %s is not a valid register' % r)
        return

    def getarg(self, arg):
        tmp1 = arg.replace(',', '')
        tmp  = tmp1.replace(' \t', '')

        if tmp[0] == '$':
            value = tmp.split('$')[1]
            return int(value), 'TYPE_IMMEDIATE'
        elif tmp[0] == '%':
            register = tmp.split('%')[1]
            return self.register_translate(register), 'TYPE_REGISTER'
        elif tmp[0] == '(':
            register = tmp.split('(')[1].split(')')[0].split('%')[1]
            return '%d,%d,%d' % (0, self.register_translate(register), self.register_translate('zero')), 'TYPE_MEMORY'
        elif tmp[0] == '.':
            targ = tmp
            return targ, 'TYPE_LABEL'
        elif tmp[0].isalpha() and not tmp[0].isdigit():
            zassert(tmp in self.vars, 'Variable %s is not declared' % tmp)
            return '%d,%d,%d' % (self.vars[tmp], self.register_translate('zero'), self.register_translate('zero')), 'TYPE_MEMORY'
        elif tmp[0].isdigit() or tmp[0] == '-':
            neg = 1
            if tmp[0] == '-':
                tmp = tmp[1:]
                neg = -1
            s = tmp.split('(')
            if len(s) == 1:
                value = neg * int(tmp)
                return '%d,%d,%d' % (int(value), self.register_translate('zero'), self.register_translate('zero')), 'TYPE_MEMORY'
            elif len(s) == 2:
                value = neg * int(s[0])
                t = s[1].split(')')[0].split(',')
                if len(t) == 1:
                    register = t[0].split('%')[1]
                    return '%d,%d,%d' % (int(value), self.register_translate(register), self.register_translate('zero')), 'TYPE_MEMORY'
                elif len(t) == 2:
                    register1 = t[0].split('%')[1]
                    register2 = t[1].split('%')[1]
                    return '%d,%d,%d' % (int(value), self.register_translate(register1), self.register_translate(register2)), 'TYPE_MEMORY'
            else:
                print('mov: bad argument [%s]' % tmp)
                exit(1)
        zassert(True, 'mov: bad argument [%s]' % arg)
        return

    #
    # LOAD a program into memory
    #
    def load(self, infile, loadaddr):
        pc   = int(loadaddr)
        fd   = open(infile)

        bpc  = loadaddr
        data = 100

        for line in fd:
            cline = line.rstrip()
            ctmp = cline.split('#')
            if len(ctmp) == 2:
                cline = ctmp[0]

            tmp = cline.split()
            if len(tmp) == 0:
                continue

            if tmp[0] == '.var':
                assert(len(tmp) == 2)
                self.vars[tmp[1]] = data
                data += 4
                zassert(data < bpc, 'Load address overrun by static data')
                if self.verbose: print('ASSIGN VAR', tmp[0], "-->", tmp[1], self.vars[tmp[1]])
            elif tmp[0][0] == '.':
                self.labels[tmp[0]] = int(pc)
                if self.verbose: print('ASSIGN LABEL', tmp[0], "-->", pc)
            else:
                pc += 1
        fd.close()

        pc = int(loadaddr)
        fd = open(infile)
        for line in fd:
            cline = line.rstrip()
            ctmp = cline.split('#')
            if len(ctmp) == 2:
                cline = ctmp[0]

            tmp = cline.split()
            if len(tmp) == 0:
                continue

            if cline[0] != '.':
                tmp              = cline.split(None, 1)
                opcode           = tmp[0]
                self.pmemory[pc] = cline.strip()

                if opcode == 'mov':
                    rtmp = tmp[1].split(',', 1)
                    arg1 = rtmp[0].strip()
                    arg2 = rtmp[1].strip()
                    (src, stype) = self.getarg(arg1)
                    (dst, dtype) = self.getarg(arg2)
                    if stype == 'TYPE_MEMORY' and dtype == 'TYPE_MEMORY':
                        exit(1)
                    elif stype == 'TYPE_IMMEDIATE' and dtype == 'TYPE_REGISTER':
                        self.memory[pc]  = 'self.move_i_to_r(%d, %d)' % (int(src), dst)
                    elif stype == 'TYPE_MEMORY'    and dtype == 'TYPE_REGISTER':
                        tmp = src.split(',')
                        self.memory[pc] = 'self.move_m_to_r(%d, %d, %d, %d)' % (int(tmp[0]), int(tmp[1]), int(tmp[2]), dst)
                    elif stype == 'TYPE_REGISTER'  and dtype == 'TYPE_MEMORY':
                        tmp = dst.split(',')
                        self.memory[pc] = 'self.move_r_to_m(%d, %d, %d, %d)' % (src, int(tmp[0]), int(tmp[1]), int(tmp[2]))
                    elif stype == 'TYPE_REGISTER'  and dtype == 'TYPE_REGISTER':
                        self.memory[pc] = 'self.move_r_to_r(%d, %d)' % (src, dst)
                    elif stype == 'TYPE_IMMEDIATE'  and dtype == 'TYPE_MEMORY':
                        tmp = dst.split(',')
                        self.memory[pc] = 'self.move_i_to_m(%d, %d, %d, %d)' % (src, int(tmp[0]), int(tmp[1]), int(tmp[2]))
                elif opcode == 'pop':
                    if len(tmp) == 1:
                        self.memory[pc] = 'self.pop()'
                    else:
                        arg = tmp[1].strip()
                        (dst, dtype) = self.getarg(arg)
                        self.memory[pc] = 'self.pop_r(%d)' % dst
                elif opcode == 'push':
                    (src, stype) = self.getarg(tmp[1].strip())
                    if stype == 'TYPE_REGISTER':
                        self.memory[pc] = 'self.push_r(%d)' % (int(src))
                    elif stype == 'TYPE_MEMORY':
                        tmp = src.split(',')
                        self.memory[pc] = 'self.push_m(%d,%d,%d)' % (int(tmp[0]), int(tmp[1]), int(tmp[2]))
                elif opcode == 'call':
                    (targ, ttype) = self.getarg(tmp[1].strip())
                    self.memory[pc] = 'self.call(%d)' % (int(self.labels[targ]))
                elif opcode == 'ret':
                    self.memory[pc] = 'self.ret()'
                elif opcode == 'add':
                    rtmp = tmp[1].split(',', 1)
                    arg1, arg2 = rtmp[0].strip(), rtmp[1].strip()
                    (src, stype), (dst, dtype) = self.getarg(arg1), self.getarg(arg2)
                    if stype == 'TYPE_IMMEDIATE':
                        self.memory[pc] = 'self.add_i_to_r(%d, %d)' % (int(src), dst)
                    else:
                        self.memory[pc] = 'self.add_r_to_r(%d, %d)' % (int(src), dst)
                elif opcode == 'sub':
                    rtmp = tmp[1].split(',', 1)
                    arg1, arg2 = rtmp[0].strip(), rtmp[1].strip()
                    (src, stype), (dst, dtype) = self.getarg(arg1), self.getarg(arg2)
                    if stype == 'TYPE_IMMEDIATE':
                        self.memory[pc] = 'self.sub_i_to_r(%d, %d)' % (int(src), dst)
                    else:
                        self.memory[pc] = 'self.sub_r_to_r(%d, %d)' % (int(src), dst)
                elif opcode == 'fetchadd':
                    rtmp = tmp[1].split(',', 1)
                    arg1, arg2 = rtmp[0].strip(), rtmp[1].strip()
                    (src, stype), (dst, dtype) = self.getarg(arg1), self.getarg(arg2)
                    tmp = dst.split(',')
                    self.memory[pc] = 'self.fetchadd(%d, %d, %d, %d)' % (src, int(tmp[0]), int(tmp[1]), int(tmp[2]))
                elif opcode == 'xchg':
                    rtmp = tmp[1].split(',', 1)
                    arg1, arg2 = rtmp[0].strip(), rtmp[1].strip()
                    (src, stype), (dst, dtype) = self.getarg(arg1), self.getarg(arg2)
                    tmp = dst.split(',')
                    self.memory[pc] = 'self.atomic_exchange(%d, %d, %d, %d)' % (src, int(tmp[0]), int(tmp[1]), int(tmp[2]))
                elif opcode == 'test':
                    rtmp = tmp[1].split(',', 1)
                    arg1, arg2 = rtmp[0].strip(), rtmp[1].strip()
                    (src, stype), (dst, dtype) = self.getarg(arg1), self.getarg(arg2)
                    if stype == 'TYPE_IMMEDIATE' and dtype == 'TYPE_REGISTER':
                        self.memory[pc] = 'self.test_i_r(%d, %d)' % (int(src), dst)
                    elif stype == 'TYPE_REGISTER' and dtype == 'TYPE_REGISTER':
                        self.memory[pc] = 'self.test_r_r(%d, %d)' % (int(src), dst)
                    else:
                        self.memory[pc] = 'self.test_r_i(%d, %d)' % (int(src), dst)
                elif opcode == 'j':
                    (targ, ttype) = self.getarg(tmp[1].strip())
                    self.memory[pc] = 'self.jump(%d)' % int(self.labels[targ])
                elif opcode == 'jne':
                    (targ, ttype) = self.getarg(tmp[1].strip())
                    self.memory[pc] = 'self.jump_notequal(%d)' % int(self.labels[targ])
                elif opcode == 'je':
                    (targ, ttype) = self.getarg(tmp[1].strip())
                    self.memory[pc] = 'self.jump_equal(%d)' % self.labels[targ]
                elif opcode == 'halt':
                    self.memory[pc] = 'self.halt()'
                elif opcode == 'yield':
                    self.memory[pc] = 'self.iyield()'
                elif opcode == 'nop':
                    self.memory[pc] = 'self.nop()'
                else:
                    self.memory[pc] = 'self.nop()'

                if self.verbose: print('pc:%d LOADING %20s --> %s' % (pc, self.pmemory[pc], self.memory[pc]))
                pc += 1
        fd.close()
        return

    def print_headers(self, procs):
        if len(self.memtrace) > 0:
            for m in self.memtrace:
                if m[0].isdigit():
                    print('%5d' % int(m), end=' ')
                else:
                    print('%5s' % m, end=' ')
            print(' ', end=' ')
        if len(self.regtrace) > 0:
            for r in self.regtrace:
                print('%5s' % self.get_regname(r), end=' ')
            print(' ', end=' ')
        if self.cctrace == True:
            print('>= >  <= <  != ==', end=' ')

        for i in range(procs.getnum()):
            print('       Thread %d        ' % i, end=' ')
        print('')

    def print_trace(self, newline):
        if len(self.memtrace) > 0:
            for m in self.memtrace:
                if self.compute:
                    if m[0].isdigit():
                        print('%5d' % self.memory[int(m)], end=' ')
                    else:
                        print('%5d' % self.memory[self.vars[m]], end=' ')
                else:
                    print('%5s' % '?', end=' ')
            print(' ', end=' ')
        if len(self.regtrace) > 0:
            for r in self.regtrace:
                if self.compute:
                    print('%5d' % self.registers[r], end=' ')
                else:
                    print('%5s' % '?', end=' ')
            print(' ', end=' ')
        if self.cctrace == True:
            for c in self.condlist:
                if self.compute:
                    print('1 ' if self.conditions[c] else '0 ', end=' ')
                else:
                    print('? ', end=' ')
        if (len(self.memtrace) > 0 or len(self.regtrace) > 0 or self.cctrace == True) and newline == True:
            print('')

    def setint(self, intfreq, intrand):
        if intrand == False:
            return intfreq
        return int(random.random() * intfreq) + 1

    def run(self, procs, intfreq, intrand):
        interrupt = self.setint(intfreq, intrand)
        icount    = 0
        self.print_headers(procs)
        self.print_trace(True)
        
        while True:
            tid = procs.getcurr().gettid()
            prevPC       = self.PC
            instruction  = self.memory[self.PC]
            self.PC     += 1

            rc = eval(instruction)
            self.print_trace(False)
            dospace(tid)
            print(prevPC, self.pmemory[prevPC])
            icount += 1

            if rc == -1:
                procs.done()
                if procs.numdone() == procs.getnum():
                    return icount
                procs.next()
                procs.restore()
                self.print_trace(False)
                for i in range(procs.getnum()):
                    print('----- Halt;Switch ----- ', end=' ')
                print('')

            interrupt -= 1
            if interrupt == 0 or rc == -2:
                interrupt = self.setint(intfreq, intrand)
                procs.save()
                procs.next()
                procs.restore()
                self.print_trace(False)
                for i in range(procs.getnum()):
                    print('------ Interrupt ------ ', end=' ')
                print('')
        return icount

class proclist:
    def __init__(self):
        self.plist  = []
        self.curr   = 0
        self.active = 0

    def done(self):
        self.plist[self.curr].setdone()
        self.active -= 1

    def numdone(self):
        return len(self.plist) - self.active

    def getnum(self):
        return len(self.plist)

    def add(self, p):
        self.active += 1
        self.plist.append(p)

    def getcurr(self):
        return self.plist[self.curr]

    def save(self):
        self.plist[self.curr].save()

    def restore(self):
        self.plist[self.curr].restore()

    def next(self):
        for i in range(self.curr+1, len(self.plist)):
            if self.plist[i].isdone() == False:
                self.curr = i
                return
        for i in range(0, self.curr+1):
            if self.plist[i].isdone() == False:
                self.curr = i
                return
            
class process:
    def __init__(self, cpu, tid, pc, stackbottom, reginit):
        self.cpu   = cpu
        self.tid   = tid
        self.pc    = pc
        self.regs  = {}
        self.cc    = {}
        self.done  = False
        self.stack = stackbottom

        for r in self.cpu.get_regnums():
            self.regs[r] = 0
        if reginit != '':
            for r in reginit.split(':'):
                tmp = r.split('=')
                self.regs[self.cpu.get_regnum(tmp[0])] = int(tmp[1])

        for c in self.cpu.get_condlist():
            self.cc[c] = False

        self.regs[self.cpu.get_regnum('sp')] = stackbottom

    def gettid(self):
        return self.tid

    def save(self):
        self.pc = self.cpu.get_pc()
        for c in self.cpu.get_condlist():
            self.cc[c] = self.cpu.get_cond(c)
        for r in self.cpu.get_regnums():
            self.regs[r] = self.cpu.get_reg(r)

    def restore(self):
        self.cpu.set_pc(self.pc)
        for c in self.cpu.get_condlist():
            self.cpu.set_cond(c, self.cc[c])
        for r in self.cpu.get_regnums():
            self.cpu.set_reg(r, self.regs[r])

    def setdone(self):
        self.done = True

    def isdone(self):
        return self.done == True

#
# main program
#
parser = OptionParser()
parser.add_option('-s', '--seed',      default=0,          type='int',    dest='seed')
parser.add_option('-t', '--threads',   default=2,          type='int',    dest='numthreads')
parser.add_option('-p', '--program',   default='',         type='string', dest='progfile')
parser.add_option('-i', '--interrupt', default=50,         type='int',    dest='intfreq')
parser.add_option('-r', '--randints',  default=False,      action='store_true', dest='intrand')
parser.add_option('-a', '--argv',      default='',         type='string', dest='argv')
parser.add_option('-L', '--loadaddr',  default=1000,       type='int',    dest='loadaddr')
parser.add_option('-m', '--memsize',   default=128,        type='int',    dest='memsize')
parser.add_option('-M', '--memtrace',  default='',         type='string', dest='memtrace')
parser.add_option('-R', '--regtrace',  default='',         type='string', dest='regtrace')
parser.add_option('-C', '--cctrace',   default=False,      action='store_true', dest='cctrace')
parser.add_option('-S', '--printstats',default=False,      action='store_true', dest='printstats')
parser.add_option('-v', '--verbose',   default=False,      action='store_true', dest='verbose')
parser.add_option('-c', '--compute',   default=False,      action='store_true', dest='solve')
(options, args) = parser.parse_args()

seed       = int(options.seed)
numthreads = int(options.numthreads)
intfreq    = int(options.intfreq)
intrand    = int(options.intrand)
progfile   = options.progfile
argv       = options.argv.split(',')
loadaddr   = options.loadaddr
memsize    = options.memsize
random.seed(seed)

memtrace = options.memtrace.split(',') if options.memtrace != '' else []
regtrace = options.regtrace.split(',') if options.regtrace != '' else []

cpu_obj = cpu(memsize, memtrace, regtrace, options.cctrace, options.solve, options.verbose)
cpu_obj.load(progfile, loadaddr)

procs = proclist()
stack = memsize * 1024
for i in range(numthreads):
    arg = argv[i] if len(argv) > i else argv[0]
    procs.add(process(cpu_obj, i, loadaddr, stack, arg))
    stack -= 1024

procs.restore()
t1 = time.perf_counter()
ic = cpu_obj.run(procs, intfreq, intrand)
t2 = time.perf_counter()

if options.printstats:
    print('\nSTATS:: Instructions    %d' % ic)
    print('STATS:: Emulation Rate  %.2f kinst/sec' % (float(ic) / float(t2 - t1) / 1000.0))