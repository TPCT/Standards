#ifndef STANDARDS_STANDARDS_H
#define STANDARDS_STANDARDS_H
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
    _APND = 04,
    _UNBUF = 010,
    _EOF = 020,
    _ERR = 040,
    _RDWR = 0100
};

int _fillBuf(FILE* file);
int _flushBuf(int character, FILE* file);
int inline fEOF(FILE* file);
int inline fERROR(FILE* file);
int inline fileNo(FILE* file);
int inline getC(FILE* file);
int inline putC(FILE* file, char x);
char putChar(char input);
char getChar(void);
char getCh(void);
String getPWD(unsigned long long passwordSize, char replacementChar);
FILE* Fopen(char *, char const *);
#endif //STANDARDS_STANDARDS_H
