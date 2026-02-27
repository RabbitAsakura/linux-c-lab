.var mutex
.var count

.main
.top	

.acquire
mov  $1, %ax        
xchg %ax, mutex   
test $0, %ax        
je .acquire_done    
yield               
j .acquire
.acquire_done

mov  count, %ax     
add  $1, %ax       
mov  %ax, count    

mov  $0, mutex

sub  $1, %bx
test $0, %bx
jgt .top	

halt