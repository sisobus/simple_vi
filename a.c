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
    FILE * fin = fopen(filename,"r");
    if ( fin != NULL ) {
        //for ( int i = 0; ~fscanf(fin,"%[^\n]\n",lines[i]) ; i++ );
        for ( int i = 0 ; fgets(lines[i],MAX_BUFFER_SIZE,fin) != NULL; i++ ) lines[i][WIDTH] = '\0';
        fclose(fin);
    }
    bool isChanged = false;
    for ( int i = 0 ; i < HEIGHT ; i++ ) 
        for ( int j = 0 ; j < WIDTH ; j++ ) 
            if ( lines[i][j] == '\0' ) lines[i][j] = ' ';


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
            } else if ( c == 'k' ) {
                if ( ~cursor.move(UP) ) {
                    int lastIndex = getIndexAtLastValue(lines[cursor.y]);
                    if ( isEmptyString(lines[cursor.y]) ) cursor.x = 0;
                    else if ( cursor.x > lastIndex ) cursor.x = lastIndex;
                } else {
                }
            } else if ( c == 'j' ) {
                int lastIndex = getIndexAtLastLine(lines);
                if ( cursor.y < lastIndex ) 
                    cursor.move(DOWN);
                lastIndex = getIndexAtLastValue(lines[cursor.y]);
                if ( isEmptyString(lines[cursor.y]) ) cursor.x = 0;
                else if ( cursor.x > lastIndex ) cursor.x = lastIndex;
            } else if ( c == 'l' ) {
                int lastIndex = getIndexAtLastValue(lines[cursor.y]);
                if ( cursor.x <= lastIndex )
                    cursor.move(RIGHT);
            } else if ( c == 'o' ) {
                cursor.move(DOWN);
                moveDownAtYIndex(lines,cursor.y);
                cursor.x = 0;
                mode = INSERT;
            } else if ( c == '0' ) {
                cursor.x = 0;
            } else if ( c == '$' ) {
                cursor.x = getIndexAtLastValue(lines[cursor.y]);
            } else if ( c == 'w' ) {
                cursor = getCursorAtNextValue(lines,cursor);
            } else {
                cursor = getCursorAtPrevValue(lines,cursor);
            }
        } else if ( mode == INSERT ) {
            if ( c == ESC ) {
                mode = COMMAND;
            } else if ( c == BACKSPACE ) {
                if ( cursor.x == 0 ) {
                    if ( ~cursor.move(UP) ) {
                        int lastIndex = getIndexAtLastValue(lines[cursor.y]);
                        if ( isEmptyString(lines[cursor.y]) ) {
                            cursor.x = 0;
                        } else {
                            cursor.x = lastIndex+1;
                        }
                        int prevLastIndex =getIndexAtLastValue(lines[cursor.y+1]);
                        copyLine(lines[cursor.y],lines[cursor.y+1],cursor.x,0,prevLastIndex+1);
                        moveUpAtYIndex(lines,cursor.y+1);
                    } else {
                    }
                }  else {
                    moveLeftAtXIndex(lines[cursor.y],cursor.x);
                    cursor.move(LEFT);
                    lines[cursor.y][cursor.x] = ' ';
                }
            } else if ( c == ENTER ) { 
                cursor.move(DOWN);
                moveDownAtYIndex(lines,cursor.y);
                copyLine(lines[cursor.y],lines[cursor.y-1],0,cursor.x,WIDTH);
                initializeLine(lines[cursor.y-1],cursor.x);
                cursor.x = 0;
            } else if ( c == TAB ) {
                int curX = cursor.x;
                for ( int i = 0 ; i < TAB_LENGTH ; i++ ) {
                    moveRightAtXIndex(lines[cursor.y],curX+i);
                    lines[cursor.y][curX+i] = ' ';
                    cursor.move(RIGHT);
                }
                for ( int i = cursor.x ; i < curX + TAB_LENGTH ; i++ ) {
                    lines[cursor.y][cursor.x+i] = ' ';
                    cursor.move(RIGHT);
                }
            } else {
                moveRightAtXIndex(lines[cursor.y],cursor.x);
                lines[cursor.y][cursor.x] = c;
                int r = cursor.move(RIGHT);
                isChanged = true;
            }
        } else if ( mode == COMMAND_COLON ) {
            if ( c == ESC ) {
                strcpy(command,"");
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
                    isChanged = false;
                } else if ( !strcmp(command,"wq") ) {
                    FILE *fp = fopen(filename,"w");
                    for ( int i = 0 ; i < HEIGHT ; i++ ) {
                        for ( int j = 0 ; j < WIDTH ; j++ ) 
                            fprintf(fp,"%c",lines[i][j]);
                        fprintf(fp,"\n");
                    }
                    fclose(fp);
                    strcpy(result,"saved and quit");
                    draw(filename,&cursor,mode,command,result);
                    return 0;
                } else if ( !strcmp(command,"q") ) {
                    if ( isChanged ) {
                        strcpy(result,"No write since last change (add ! to override)");
                    } else {
                        strcpy(result,"quit");
                        draw(filename,&cursor,mode,command,result);
                        return 0;
                    }
                } else if ( !strcmp(command,"q!") ) {
                    strcpy(result,"! quit");
                    draw(filename,&cursor,mode,command,result);
                    return 0;
                } else if ( isDigit(command) ) {
                    int t = str2int(command);
                    int lastLineIndex = getIndexAtLastLine(lines);
                    cursor.y = t>=(lastLineIndex)?(lastLineIndex):t;
                } else {
                    strcpy(result,"command not found");
                }
                strcpy(command,"");
                mode = COMMAND;
            } else if ( c == BACKSPACE ) {
                if ( strlen(command) == 0 ) {
                    mode = COMMAND;
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
