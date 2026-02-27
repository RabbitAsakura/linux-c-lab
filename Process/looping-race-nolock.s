.main
.top	

mov 2000, %ax  
add $1, %ax    
mov %ax, 2000  


sub  $1, %bx
test $0, %bx
jgt .top	

halt