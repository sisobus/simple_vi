#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <termio.h>
int getch(void){
    int ch;
    struct termios buf, save;
    tcgetattr(0,&save);
    buf = save;
    buf.c_lflag &= ~(ICANON|ECHO);
    buf.c_cc[VMIN] = 1;
    buf.c_cc[VTIME] = 0;
    tcsetattr(0, TCSAFLUSH, &buf);
    ch = getchar();
    tcsetattr(0, TCSAFLUSH, &save);
    return ch;
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
#define ESC 27
char lines[HEIGHT][WIDTH];

typedef enum _Dir {
    UP=65,RIGHT,DOWN,LEFT
} Dir;

typedef struct Cursor {
    int y,x;
    Cursor(){}
    Cursor(int _y,int _x):
        y (_y), x (_x) {}
    void move(Dir d) {
        if ( d == UP ) {
            if ( y <= 0 ) return;
            y--;
        } else if ( d == RIGHT ) {
            if ( x >= WIDTH-2 ) return ;
            x++;
        } else if ( d == DOWN ) {
            if ( y >= HEIGHT-1 ) return;
            y++;
        } else if ( d == LEFT ) {
            if ( x <= 0 ) return ;
            x--;
        }
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
void draw(char **lines,Cursor* cursor,MODE m,char *command);
