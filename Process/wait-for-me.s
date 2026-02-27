.main
test $1, %ax    
je .signaller

.waiter	
mov  2000, %cx
test $1, %cx
jne .waiter
halt

.signaller
mov  $1, 2000
halt