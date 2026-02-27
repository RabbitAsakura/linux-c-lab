.var ticket
.var turn
.var count

.main
.top	

.acquire
mov $1, %ax
fetchadd %ax, ticket 
.tryagain
mov turn, %cx         
test %cx, %ax
jne .tryagain

# critical section
mov  count, %ax       
add  $1, %ax         
mov  %ax, count     

mov $1, %ax
fetchadd %ax, turn

sub  $1, %bx
test $0, %bx
jgt .top	

halt