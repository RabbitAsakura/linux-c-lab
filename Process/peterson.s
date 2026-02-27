.var flag   2     
.var turn
.var count
.main


lea flag, %fx


mov %bx, %cx   
neg %cx        
add $1, %cx    

.acquire
mov $1, 0(%fx,%bx,4)   
mov %cx, turn          

.spin1
mov 0(%fx,%cx,4), %ax   
test $1, %ax            
jne .fini               

.spin2                 
mov turn, %ax
test %cx, %ax           
je .spin1              

.fini

mov count, %ax
add $1, %ax
mov %ax, count

mov $0, 0(%fx,%bx,4)    # flag[self] = 0


mov %cx, turn           # turn       = 1 - self
halt