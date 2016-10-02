#include "a.h"
int main(int argc,char *argv[]) {
    assert(argc >= 2);
    char *filename = argv[1];
    Cursor cursor(0,0);
    MODE mode = COMMAND;
    char *command = (char*)malloc(sizeof(char)*MAX_COMMAND_SIZE);
    memset(command,'\0',sizeof(command));

    while ( true ) {
        draw(filename,&cursor,mode,command);
        int c = getch();
        if ( mode == COMMAND ) {
            if ( c == ESC ) {
                continue;
            } else if ( c == ':' ) {
                mode = COMMAND_COLON;
            } else if ( c == 'i' ) {
                mode = INSERT;
            }
        } else if ( mode == INSERT ) {
            if ( c == ESC ) {
                mode = COMMAND;
            } else if ( c == UP ) {
                cursor.move(UP);
            } else if ( c == RIGHT ) {
                cursor.move(RIGHT);
            } else if ( c == DOWN ) {
                cursor.move(DOWN);
            } else if ( c == LEFT ) {
                cursor.move(LEFT);
            } else {
                lines[cursor.y][cursor.x] = c;
            }
        } else if ( mode == COMMAND_COLON ) {
            if ( c == ESC ) {
                mode = COMMAND;
            }
        }
    }
    return 0;
}
void draw(char *filename,Cursor* cursor,MODE m,char *command) {
    system("clear");
    for ( int i = 0 ; i < HEIGHT ; i++ ) {
        for ( int j = 0 ; j < WIDTH ; j++ ) {
            if ( cursor.y == i && cursor.x == j ) {
                printf("I");
            } else {
                printf("%c",lines[i][j]);
            }
        }
        puts("");
    }
    if ( m == COMMAND ) {
        printf("[NORMAL] > ./%s > [Cursor]%d:%d",filename,cursor.y,cursor.x);
        printf("-- COMMAND --\n");
    } else if ( m == INSERT ) {
        printf("[INSERT] > ./%s > [Cursor]%d:%d",filename,cursor.y,cursor.x);
        printf("-- INSERT --\n");
    }
}
