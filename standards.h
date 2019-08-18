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
    int buffCount;
    char *nextCh;
    char *buffBase;
    int bufferLength;
} FILE;

extern FILE files[FOPEN_MAX];

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

int _fillBuf(FILE* file);
int _flushBuf(int character, FILE* file);

char inline fEOF(FILE *file);

char inline fERROR(FILE *file);

char inline fileNo(FILE *file);

char inline getC(FILE *file);

char inline putC(FILE *file, int x);

void inline fflush(FILE *file);

int putChar(char input);

int getChar(void);

int getCh(void);
String getPWD(unsigned long long passwordSize, char replacementChar);
FILE* Fopen(char *, char const *);

#endif
