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
#define TAB 9

#define TAB_LENGTH 4

#define isWhiteSpace(c) (((c)==' ')||((c)=='\0')||((c)=='\t'))
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
void initializeLine(char *s) {
    for ( int i = 0 ; i < WIDTH ; i++ ) 
        s[i] = ' ';
}
void initializeLine(char *s,int x) {
    for ( int i = x ; i < WIDTH ; i++ ) 
        s[i] = ' ';
}
int getIndexAtFirstValue(char *s) {
    for ( int i = 0 ; s[i] ; i++ ) 
        if ( s[i] != ' ' && s[i] != '\0' ) 
            return i;
    return -1;
}
int getIndexAtLastValue(char *s) {
    for ( int i = WIDTH-1 ; i >= 0 ; i-- ) 
        if ( s[i] != ' ' && s[i] != '\0' ) 
            return i;
    return -1;
}
int isEmptyString(char *s) {
    return getIndexAtLastValue(s) == -1;
}
int getIndexAtLastLine(char s[][WIDTH]) {
    for ( int i = HEIGHT-1 ; i >= 0 ; i-- ) 
        if ( !isEmptyString(s[i]) ) return i;
    return -1;
}
int isDigit(char *s) {
    for ( int i = 0 ; s[i] ; i++ ) 
        if ( !(s[i] >= '0' && s[i] <= '9') ) return false;
    return true;
}
int isLower(char c) {
    return 'a' <= c && c <= 'z';
}
int isUpper(char c) {
    return 'A' <= c && c <= 'Z';
}
int isAlphabet(char c) {
    return isLower(c) || isUpper(c);
}
int isNumber(char c) {
    return '0' <= c && c <= '9';
}
int isOther(char c) {
    return !isNumber(c) && !isWhiteSpace(c) && !isAlphabet(c);
}
int str2int(char *s) {
    int ret;
    sscanf(s,"%d",&ret);
    return ret;
}
void copyLine(char *s1,char *s2) {
    for ( int i = 0 ; i < WIDTH ; i++ ) 
        s1[i] = s2[i];
}
void copyLine(char *s1,char *s2,int s1i,int s2i,int len) {
    for ( int i = 0 ; i < len && s1i+i < WIDTH && s2i+i < WIDTH ; i++ ) 
        s1[s1i+i] = s2[s2i+i];
}
int moveDownAtYIndex(char lines[][WIDTH],int y) {
    int lastIndex = getIndexAtLastLine(lines);
    if ( lastIndex + 1 >= HEIGHT ) return -1;
    for ( int i = lastIndex+1 ; i > y ; i-- ) 
        copyLine(lines[i],lines[i-1]);
    initializeLine(lines[y]);
    return 1;
}
int moveRightAtXIndex(char *line,int x) {
    int lastIndex = getIndexAtLastValue(line);
    if ( lastIndex + 1 >= WIDTH ) return -1;
    for ( int i = lastIndex+1 ; i > x ; i-- ) 
        line[i] = line[i-1];
    line[x] = ' ';
    return 1;
}
int moveLeftAtXIndex(char *line,int x) {
    int lastIndex = getIndexAtLastValue(line);
    if ( x > lastIndex ) return -1;
    for ( int i = x ; i < lastIndex ; i++ ) 
        line[i] = line[i+1];
    line[lastIndex] = ' ';
    return 1;
}
int moveUpAtYIndex(char lines[][WIDTH],int y) {
    int lastIndex = getIndexAtLastLine(lines);
    if ( y > lastIndex ) return -1;
    for ( int i = y ; i < lastIndex ; i++ ) 
        copyLine(lines[i],lines[i+1]);
    initializeLine(lines[lastIndex]);
    return 1;
}
Cursor getCursorAtNextValue(char lines[][WIDTH],Cursor cursor) {
    int lastLineIndex = getIndexAtLastLine(lines);
    for ( int i = cursor.x ; i < WIDTH-1 ; i++ ) {
        char prev = lines[cursor.y][i];
        char next = lines[cursor.y][i+1];
        if ( 
                (isNumber(prev) && isOther(next)) ||
                (isWhiteSpace(prev) && isNumber(next)) ||
                (isWhiteSpace(prev) && isAlphabet(next)) ||
                (isWhiteSpace(prev) && isOther(next)) ||
                (isAlphabet(prev) && isOther(next)) ||
                (isOther(prev) && isNumber(next)) ||
                (isOther(prev) && isAlphabet(next)) 
                )
            return Cursor(cursor.y,i+1);
    }
    int lastIndex = getIndexAtLastValue(lines[cursor.y]);
    if ( lastLineIndex == cursor.y ) return Cursor(cursor.y,lastIndex);
    for ( int i = cursor.y+1 ; i <= lastLineIndex ; i++ ) {
        if ( isEmptyString(lines[i]) ) continue;
        return Cursor(i,getIndexAtFirstValue(lines[i]));
    }
    return Cursor(cursor.y,lastIndex);
}
int getIndexAtFirstIndexInAlphabet(char *s,int x) {
    if ( !isAlphabet(s[x]) ) return -1;
    while ( x >= 0 && isAlphabet(s[x]) ) x--;
    return x+1;
}
int getIndexAtFirstIndexInNumber(char *s,int x) {
    if ( !isNumber(s[x]) ) return -1;
    while ( x >= 0 && (isNumber(s[x]) || isAlphabet(s[x])) ) x--;
    return x+1;
}
int getIndexAtFirstIndexInOther(char *s,int x) {
    if ( !isOther(s[x]) ) return -1;
    while ( x >= 0 && isOther(s[x]) ) x--;
    return x+1;
}
Cursor getCursorAtPrevValue(char lines[][WIDTH],Cursor cursor) {
     for ( int i = cursor.x-1 ; i >= 0 ; i-- ) {
        char prev = lines[cursor.y][i];
        char next = lines[cursor.y][i+1];
        if ( 
                (isNumber(prev) && isOther(next)) ||
                (isWhiteSpace(next) && isNumber(prev)) ||
                (isWhiteSpace(next) && isAlphabet(prev)) ||
                (isWhiteSpace(next) && isOther(prev)) ||
                (isAlphabet(prev) && isOther(next)) ||
                (isOther(prev) && isNumber(next)) ||
                (isOther(prev) && isAlphabet(next)) 
                ) {
            int prevFirstIndex;
            if ( isNumber(prev) ) {
                prevFirstIndex = getIndexAtFirstIndexInNumber(lines[cursor.y],i);
            } else if ( isAlphabet(prev) ) {
                prevFirstIndex = getIndexAtFirstIndexInAlphabet(lines[cursor.y],i);
            } else if ( isOther(prev) ) {
                prevFirstIndex = getIndexAtFirstIndexInOther(lines[cursor.y],i);
            }
            return Cursor(cursor.y,prevFirstIndex);
        }
    }
    int firstIndex = getIndexAtFirstValue(lines[cursor.y]);
    if ( 0 == cursor.y ) return Cursor(cursor.y,firstIndex);
    for ( int i = cursor.y-1 ; i >= 0 ; i-- ) {
        if ( isEmptyString(lines[i]) ) continue;
        return Cursor(i,getIndexAtLastValue(lines[i]));
    }
    return Cursor(cursor.y,firstIndex);
   
}
