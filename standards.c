#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "standards.h"

char fEOF(FILE *file) {
    return ((file->flag & _EOF) != 0);
}

char fERROR(FILE *file) {
    return ((file->flag & _ERR) != 0);
}

char fileNo(FILE *file) {
    return (file->fd);
}

char getC(FILE *file) {
    return (--(file->buffCount) >= 0 ? *(file->nextCh++) : _fillBuf(file));
}

char putC(FILE *file, int x) {
    return ((file->nextCh == 0 ? file->buffCount++ : 0) <= BUFSIZ ? (*(file->nextCh++) = x) : _flushBuf(x, file));
}

FILE files[FOPEN_MAX] = {{0, _READ, 0, (char *)0, (char *)0},
                         {1, _WRITE, 0, (char *)0, (char *)0},
                         {2, _WRITE, 0, (char *)0, (char *)0}};

void fflush(FILE *file) {
    _flushBuf(-100, file);
}
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

int putChar(char input) {
    char data = write(0, &input, 1);
    return data;
}

int getChar(void) {
    char c;
    char data = read(0, &c, 1);
    if (data != -1)
        return c;
    return data;
}

int getCh(void) {
    initTerminal(0);
    char c;
    c = getChar();
    resetTerminal();
    return c;
}

int _fillBuf(FILE *file) {
    if ((file->flag & (_READ | _EOF | _ERR)) != _READ)
        return EOF;
    if (file->buffBase == NULL)
        if ((file->buffBase = (char *) calloc(BUFSIZ, sizeof(char))) == NULL)
            return EOF;
    for (int i = 0; (file->buffBase)[i] != 0; (file->buffBase)[i++] = 0);
    lseek(file->fd, -file->bufferLength, SEEK_CUR);
    write(file->fd, file->buffBase, file->buffCount);
    lseek(file->fd, file->bufferLength, SEEK_CUR);
    file->nextCh = file->buffBase;
    file->bufferLength = file->buffCount = read(file->fd, file->buffBase, BUFSIZ);
    if (--file->buffCount < 0) {
        if (file->buffCount == -1)
            file->flag |= _EOF;
        else
            file->flag |= _ERR;
        file->buffCount = 0;
        return EOF;
    }
    return *file->nextCh++;
}

int _flushBuf(int x, FILE *file) {
    if ((file->flag & (_WRITE | _EOF | _ERR)) != _WRITE)
        return EOF;
    if (file->buffBase == NULL)
        return EOF;
    char validString[BUFSIZ];
    int i = 0;
    for (; (file->buffBase)[i] != 0; validString[i] = (file->buffBase)[i], (file->buffBase)[i++] = 0);
    write(file->fd, validString, i);
    file->nextCh = file->buffBase;
    if (x != -100)
        *file->nextCh++ = x;
    return x;
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
    for(; *(mode + modeLength); modeLength++);
    const char MODE = tolower(*mode);
    if (MODE != 'r' && MODE != 'w' && MODE != 'a')
        return NULL;
    char RDWR = 0;
    if (modeLength > 1){
        if (*(mode + 1) == 'b'){
            if (*(mode + 2) == '+')
                RDWR = 1;
            else
                return NULL;
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
            fd = creat(FileName, PERMS);
            returnFile->flag = RDWR ? _RDWR : _WRITE;
            break;
        case 'r':
            if (RDWR) {
                fd = open(FileName, O_RDWR);
                returnFile->flag = _RDWR;
            } else {
                fd = open(FileName, O_RDONLY);
                returnFile->flag = _READ;
            }
            if (fd == -1)
                return NULL;
            break;
        case 'a':
            fd = open(FileName, O_RDWR | O_APPEND | O_CREAT);
            returnFile->flag = _WRITE;
            break;
        default:
            break;
    }
    if (fd == -1)
        return NULL;
    returnFile->fd = fd;
    returnFile->buffBase = (char *) calloc(BUFSIZ, sizeof(char));
    returnFile->nextCh = returnFile->buffBase;
    return returnFile;
}


