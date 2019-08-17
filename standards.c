#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "standards.h"

int inline fEOF(FILE * file){
    return ((file->flag & _EOF) != 0);
}

int inline fERROR(FILE *file){
    return ((file->flag & _ERR) != 0);
}

int inline fileNo(FILE *file){
    return (file->fd);
}

int inline getC(FILE *file){
    return (--(file->buffCount) >= 0 ? *(file->nextCh++) : _fillBuf(file));
}

int inline putC(FILE *file, char x){
    return (file->buffCount < BUFSIZ ? *(file->nextCh++) = x : _flushBuf(x, file));
}

FILE files[FOPEN_MAX] = {{0, _READ, 0, (char *)0, (char *)0},
                         {1, _WRITE, 0, (char *)0, (char *)0},
                         {2, _WRITE, 0, (char *)0, (char *)0}};

static struct termios newTerminal, oldTerminal;

static void initTerminal(char Echoing){
    tcgetattr(0, &oldTerminal);
    newTerminal = oldTerminal;
    newTerminal.c_lflag &= ~ICANON;
    newTerminal.c_lflag &= Echoing ? ECHO : ~ECHO;
    tcsetattr(0, TCSANOW, &newTerminal);
}

static void resetTerminal(void){
    tcsetattr(0, TCSANOW, &oldTerminal);
}

char putChar(char input){
    char data = write(0, &input, 1);
    return data;
}

char getChar (void){
    char c;
    char data = read(0, &c, 1);
    if (data != -1)
        return c;
    return data;
}

char getCh(void){
    initTerminal(0);
    char c;
    c = getChar();
    resetTerminal();
    return c;
}

String getPWD(unsigned long long passwordSize, char replacementChar){
    String data = (String) calloc(passwordSize, sizeof(char));
    unsigned long long passwordCounter = 0;
    unsigned character = 0;
    while(passwordCounter < passwordSize &&
          ((character = getCh()) != EOF) && character != '\n'){
        if(character == newTerminal.c_cc[VERASE]){
            write(0, "\b \b", 3);
            //fflush(stdin);
            data[passwordCounter ? --passwordCounter : passwordCounter] = '\0';
            continue;
        }
        data[passwordCounter++] = character;
        putChar(replacementChar);
    }
    putChar('\n');
    return data;
}

FILE * Fopen(char *FileName, char const* mode){
    char fd = 0;
    FILE* returnFile;
    if (!mode)
        return NULL;
    char modeLength = 0;
    char createNewFile = 0;
    for(; *(mode + modeLength); modeLength++);
    const char MODE = tolower(*mode);
    if (MODE != 'r' && MODE != 'w' && MODE != 'a')
        return NULL;
    char RDWR = 0;
    if (modeLength > 1){
        if (*(mode + 1) == 'b'){
            if (*(mode + 2) == '+')
                RDWR = 1;
        }else if (*(mode + 1) == '+')
            RDWR = 1;
        else
            return NULL;
    }
    for (returnFile = files; returnFile < files + FOPEN_MAX; returnFile++)
        if (!returnFile->flag)
            break;
    if (returnFile == files + FOPEN_MAX)
        return NULL;
    switch (MODE) {
        case 'w':
            fd = creat(FileName, S_IWGRP | S_IWOTH | S_IWUSR);
            returnFile->flag = _WRITE;
            break;
        case 'r':
            fd = open(FileName, O_RDONLY);
            if (fd == -1)
                return NULL;
            returnFile->flag = createNewFile ? _WRITE : _READ;
            break;
        case 'a':
            fd = open(FileName, O_RDWR | O_APPEND | O_CREAT);
            returnFile->flag = _APND;
            break;
        default:
            break;
    }
    if (fd == -1)
        return NULL;
    returnFile->fd = fd;
    returnFile->flag = RDWR ? _RDWR : returnFile->flag;
    returnFile->buffCount = 0;
    returnFile->nextCh = NULL;
    returnFile->buffBase = NULL;
}
