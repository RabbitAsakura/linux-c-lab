#ifndef PC-HEADER_H
#define PC-HEADER_H

#define MAX_THREADS (100)  

int producers = 1;         
int consumers = 1;         

int *buffer;              
int max;                   

int use_ptr  = 0;          
int fill_ptr = 0;          
int num_full = 0;         

int loops;                 

#define EMPTY         (-2) 
#define END_OF_STREAM (-1)

#endif 