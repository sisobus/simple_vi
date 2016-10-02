#include "a.h"
int main(int argc,char *argv[]) {
    assert(argc >= 2);

    char *filename = argv[1];
    Cursor cursor(0,0);
    MODE mode = COMMAND;
    char *command = (char*)malloc(sizeof(char)*MAX_COMMAND_SIZE);
    strcpy(command,"");
    char *buf = (char*)malloc(sizeof(char)*MAX_BUFFER_SIZE);
    strcpy(buf,"");
    char *result= (char*)malloc(sizeof(char)*MAX_BUFFER_SIZE);
    strcpy(result,"");

    for ( int i = 0 ; i < HEIGHT ; i++ ) 
        for ( int j = 0 ; j < WIDTH ; j++ ) 
            lines[i][j] = ' ';

    while ( true ) {
        draw(filename,&cursor,mode,command,result);
        char c = getch();
        if ( mode == COMMAND ) {
            if ( c == ESC ) {
                continue;
            } else if ( c == ':' ) {
                mode = COMMAND_COLON;
            } else if ( c == 'i' ) {
                mode = INSERT;
            } else if ( c == 'h' ) {
                cursor.move(LEFT);
            } else if ( c == 'j' ) {
                cursor.move(UP);
            } else if ( c == 'k' ) {
                cursor.move(DOWN);
            } else if ( c == 'l' ) {
                cursor.move(RIGHT);
            }
        } else if ( mode == INSERT ) {
            if ( c == ESC ) {
                mode = COMMAND;
            } else if ( c == BACKSPACE ) {
                if ( cursor.x == 0 ) {
                    continue; // 지우고 이전 줄로 옮기는거
                }  else {
                    cursor.move(LEFT);
                    lines[cursor.y][cursor.x] = ' ';
                }
            } else if ( c == ENTER ) { 
                cursor.move(DOWN);
                cursor.x = 0;
            } else {
                lines[cursor.y][cursor.x] = c;
                int r = cursor.move(RIGHT);
            }
        } else if ( mode == COMMAND_COLON ) {
            if ( c == ESC ) {
                mode = COMMAND;
            } else if ( c == ENTER ) {
                /*
                 * command process
                 */
                if ( !strcmp(command,"w") ) {
                    FILE *fp = fopen(filename,"w");
                    for ( int i = 0 ; i < HEIGHT ; i++ ) {
                        for ( int j = 0 ; j < WIDTH ; j++ ) 
                            fprintf(fp,"%c",lines[i][j]);
                        fprintf(fp,"\n");
                    }
                    fclose(fp);
                    strcpy(result,"saved");
                } else if ( !strcmp(command,"wq") ) {
                    FILE *fp = fopen(filename,"w");
                    for ( int i = 0 ; i < HEIGHT ; i++ ) {
                        for ( int j = 0 ; j < WIDTH ; j++ ) 
                            fprintf(fp,"%c",lines[i][j]);
                        fprintf(fp,"\n");
                    }
                    fclose(fp);
                    strcpy(result,"quit");
                    return 0;
                } else if ( !strcmp(command,"q") ) {
                } else if ( !strcmp(command,"q!") ) {
                }
                strcpy(command,"");
            } else if ( c == BACKSPACE ) {
                if ( strlen(command) == 0 ) {
                } else {
                    command[strlen(command)-1] = '\0';
                }
            } else {
                sprintf(buf,"%c",c);
                strcat(command,buf);
            }
        }
    }
    return 0;
}
void draw(char *filename,Cursor* cursor,MODE m,char *command,char *result) {
    system("clear");
    printf("┏");
    for ( int i = 0 ; i < WIDTH ; i++ ) 
        printf("━");
    printf("┓\n");
    for ( int i = 0 ; i < HEIGHT ; i++ ) {
        printf("┃");
        for ( int j = 0 ; j < WIDTH ; j++ ) {
            if ( cursor->y == i && cursor->x == j ) {
                printf("I");
            } else {
                printf("%c",lines[i][j]);
            }
        }
        printf("┃");
        puts("");
    }
    printf("┗");
    for ( int i = 0 ; i < WIDTH ; i++ ) 
        printf("━");
    printf("┛\n");
    if ( m == COMMAND ) {
        printf("[NORMAL] > ./%s > [Cursor]%d:%d\n",filename,cursor->y,cursor->x);
        printf("-- COMMAND --");
    } else if ( m == INSERT ) {
        printf("[INSERT] > ./%s > [Cursor]%d:%d\n",filename,cursor->y,cursor->x);
        printf("-- INSERT --");
    } else if ( m == COMMAND_COLON ) {
        printf("[NORMAL] > ./%s > [Cursor]%d:%d\n",filename,cursor->y,cursor->x);
        printf(":%s",command);
    }
    if ( strlen(result) != 0 ){
        printf("\nsystem> %s\n",result);
        strcpy(result,"");
    }
}
