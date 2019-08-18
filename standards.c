#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "standards.h"

char fEOF(SFILE *file) {
    return ((file->flag & _EOF) != 0);
}

char fERROR(SFILE *file) {
    return ((file->flag & _ERR) != 0);
}

char fileNo(SFILE *file) {
    return (file->fd);
}

char getC(SFILE *file) {
    return (((file->nextCh) && ((file->bufCounter++) < BUFSIZ)) ? *(file->nextCh++) : _fillBuf(file));
}

char putC(SFILE *file, int x) {
    if (file->buffBase == NULL) {
        if ((file->buffBase = (char *) calloc(BUFSIZ, sizeof(char))) == NULL)
            return 0;
        file->nextCh = file->buffBase;
    }
    return (*file->nextCh ? file->bufCounter : file->bufCounter++) < BUFSIZ ? (*file->nextCh++ = x) : _flushBuf(x,
                                                                                                                file);
}

SFILE files[FOPEN_MAX] = {{0, _READ,  0, (char *) 0, (char *) 0, 0},
                          {1, _WRITE, 0, (char *) 0, (char *) 0, 0},
                          {2, _WRITE, 0, (char *) 0, (char *) 0, 0}};

void Fflush(SFILE *file) {
    if ((file->flag & (_WRITE | _ERR)) != _WRITE)
        return;
    if (file->buffBase == NULL)
        return;
    char valid[BUFSIZ];
    int i = 0;
    for (; file->buffBase[i]; (valid[i] = file->buffBase[i]), (file->buffBase[i] = 0), i++);
    write(file->fd, valid, i);
    file->bufCounter = 0;
    file->nextCh = file->buffBase;
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
    if (input == '\n') {
        putC(stdout, input);
        Fflush(stdout);
    }
    return putC(stdout, input);
}

int getChar(void) {
    return getC(stdin);
}

int getCh(void) {
    initTerminal(0);
    char c;
    c = getChar();
    resetTerminal();
    return c;
}

int _fillBuf(SFILE *file) {
    if ((file->flag & (_READ | _EOF | _ERR)) != _READ)
        return EOF;
    if (file->buffBase == NULL) {
        if ((file->buffBase = (char *) calloc(BUFSIZ, sizeof(char))) == NULL)
            return EOF;
    }
    if (file->bufferLength) {
        lseek(file->fd, -file->bufferLength, SEEK_CUR);
        write(file->fd, file->buffBase, file->bufCounter);
        lseek(file->fd, file->bufferLength, SEEK_CUR);
    }
    file->nextCh = file->buffBase;
    for (int i = 0; (file->buffBase)[i] != 0 && (file->buffBase)[i] != EOF; (file->buffBase)[i++] = 0);
    file->bufferLength = read(file->fd, file->buffBase, BUFSIZ);
    if (file->bufferLength > 0)
        lseek(file->fd, -file->bufferLength, SEEK_CUR);
    else {
        if (!file->bufferLength)
            file->flag |= _EOF;
        else
            file->flag |= _ERR;
        lseek(file->fd, 0L, SEEK_END);
        return EOF;
    }
    return *file->nextCh || *file->nextCh != EOF ? *file->nextCh++ : *file->nextCh;
}

int _flushBuf(int x, SFILE *file) {
    if ((file->flag & (_WRITE | _ERR)) != _WRITE)
        return EOF;
    if (file->buffBase == NULL)
        return EOF;
    char valid[BUFSIZ];
    int i = 0;
    for (; file->buffBase[i]; (valid[i] = file->buffBase[i]), file->buffBase[i] = 0, i++);
    write(file->fd, valid, i);
    file->bufCounter = 0;
    if (x != -100)
        *file->buffBase = x;
    file->nextCh = file->buffBase;
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

SFILE *Fopen(char *FileName, char const *mode) {
    char fd = 0;
    SFILE *returnFile;
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
    returnFile->bufCounter = 0;
    returnFile->bufferLength = 0;
    returnFile->buffBase = NULL;
    _fillBuf(returnFile);
    returnFile->nextCh = returnFile->buffBase;
    return returnFile;
}


