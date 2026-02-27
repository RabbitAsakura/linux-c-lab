.var mutex
.var count

.main
.top	

.acquire
mov  mutex, %ax
test $0, %ax
jne .acquire
mov  $1, %ax        
xchg %ax, mutex     
test $0, %ax       
jne .acquire       

mov  count, %ax     
add  $1, %ax       
mov  %ax, count    

mov  $0, mutex

sub  $1, %bx
test $0, %bx
jgt .top	

halt