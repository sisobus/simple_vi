#include <stdio.h>                                                              
int dy[]={-1,0,1,0};                                                            
int dx[]={0,1,0,-1};                                                            
int W,H;                                                                        
int a[55][55];                                                                  
void go(int y,int x) {                                                          
    a[y][x] = 0;                                                                
    for ( int i = 0 ; i < 4 ; i++ ) {                                           
        int ny = y + dy[i];                                                     
        int nx = x + dx[i];                                                     
        if ( ny < 0 || nx < 0 || ny >= H || nx >= W ) continue;                 
        if ( a[ny][nx] != 0 ) cotinue;                                          
        go(ny,nx);                                                              
    }                                                                           
}                                                                               
int main() {                                                                    
    for ( int i = 0 ; i < H ; i++ )                                             
        for ( int j = 0 ; j < W ; j++ )                                         
            if ( a[i][j] == 1 )                                                 
                go(i,j);                                                        
    return 0;                                                                   
}                                                                               
                                                                                
                                                                                
