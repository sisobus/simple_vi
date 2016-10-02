#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <termio.h>
char getch(){
    char buf=0;
    struct termios old={0};
    fflush(stdout);
    if(tcgetattr(0, &old)<0)
        perror("tcsetattr()");
    old.c_lflag&=~ICANON;
    old.c_lflag&=~ECHO;
    old.c_cc[VMIN]=1;
    old.c_cc[VTIME]=0;
    if(tcsetattr(0, TCSANOW, &old)<0)
        perror("tcsetattr ICANON");
    if(read(0,&buf,1)<0)
        perror("read()");
    old.c_lflag|=ICANON;
    old.c_lflag|=ECHO;
    if(tcsetattr(0, TCSADRAIN, &old)<0)
        perror ("tcsetattr ~ICANON");
    return buf;
}

#ifndef false
#define false 0
#endif
#ifndef true
#define true !(false)
#endif
#define WIDTH   80
#define HEIGHT  24
#define MAX_COMMAND_SIZE 128
#define MAX_BUFFER_SIZE 128

#define ESC 27
#define BACKSPACE 127
#define ENTER 10
char lines[HEIGHT][WIDTH];

typedef enum _Dir {
    UP=65,DOWN,RIGHT,LEFT
} Dir;

typedef struct Cursor {
    int y,x;
    Cursor(){}
    Cursor(int _y,int _x):
        y (_y), x (_x) {}
    int move(Dir d) {
        if ( d == UP ) {
            if ( y <= 0 ) return -1;
            y--;
        } else if ( d == RIGHT ) {
            if ( x >= WIDTH-2 ) return -1;
            x++;
        } else if ( d == DOWN ) {
            if ( y >= HEIGHT-1 ) return -1;
            y++;
        } else if ( d == LEFT ) {
            if ( x <= 0 ) return -1;
            x--;
        } 
        return 1;
    }
    void print() {
        printf("%d %d\n",y,x);
    }
} Cursor;
typedef enum _MODE {
    COMMAND,INSERT,COMMAND_COLON
} MODE;
typedef enum _ERROR {
    SUCCESS = INT_MIN,
    END_OF_PROGRAM
} ERROR;
void draw(char *filename,Cursor* cursor,MODE m,char *command,char *result);
