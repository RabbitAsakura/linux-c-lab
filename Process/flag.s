.var flag
.var count

.main
.top

.acquire
mov  flag, %ax     
test $0, %ax        
jne  .acquire       
mov  $1, flag       


mov  count, %ax   
add  $1, %ax        
mov  %ax, count     

mov  $0, flag       

sub  $1, %bx
test $0, %bx
jgt .top	

halt