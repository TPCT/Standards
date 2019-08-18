#ifndef TESTING_PROJECT_STD_H
#define TESTING_PROJECT_STD_H
#define PERMS 0666
#ifndef NULL
#define NULL (void *)0
#endif
#ifndef EOF
#define EOF (-1)
#endif
#ifndef BUFSIZ
#define BUFSIZ 4096
#endif
#ifndef FOPEN_MAX
#define FOPEN_MAX 20
#endif

typedef char* String;

typedef struct _io{
    int fd;
    int flag;
    int bufCounter;
    char *nextCh;
    char *buffBase;
    int bufferLength;
} SFILE;

extern SFILE files[FOPEN_MAX];

#ifndef stdin
#define stdin &files[0]
#endif
#ifndef stdout
#define stdout &files[1]
#endif
#ifndef stderr
#define stderr &files[2]
#endif

enum _flags{
    _READ = 01,
    _WRITE = 02,
    _RDWR = 03,
    _EOF = 010,
    _ERR = 020
};

unsigned putString(SFILE *file, String stringToWrite);

int _fillBuf(SFILE *file);

int _flushBuf(int character, SFILE *file);

char inline fEOF(SFILE *file);

char inline fERROR(SFILE *file);

char inline fileNo(SFILE *file);

char inline getC(SFILE *file);

char inline putC(SFILE *file, int x);

int Fseek(SFILE *file, long position, char origin);

void inline Fflush(SFILE *file);

void Fclose(SFILE *file);

int putChar(char input);

int getChar(void);

int getCh(void);
String getPWD(unsigned long long passwordSize, char replacementChar);

SFILE *Fopen(char *, char const *);

#endif
