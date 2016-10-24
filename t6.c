#include <stdio.h>                                                              
void f(int n) {                                                                 
    if ( n <= 1 ) return 1;                                                     
    return f(n-1)+f(n-2);                                                       
}                                                                               
int main() {                                                                    
    printf("%d\n",f(10));                                                       
    return 0;                                                                   
}                                                                               
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
