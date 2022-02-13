#define HEAPSIZE 10000000
#define BUFSIZE 256
#define SYMSIZE 256
#define NIL 0
#define _CRT_SECURE_NO_WARNINGS

//-------error code---
#define CANT_FIND_ERR   1
#define ARG_SYM_ERR     2
#define ARG_NUM_ERR     3
#define ARG_LIS_ERR     4
#define ARG_LEN0_ERR    5
#define ARG_LEN1_ERR    6
#define ARG_LEN2_ERR    7
#define ARG_LEN3_ERR    8
#define MALFORM_ERR     9
#define CANT_READ_ERR   10
#define ILLEGAL_OBJ_ERR 11
#define DIV_BY_ZERO     12



#include <stdio.h>
#include<stdlib.h>
#include <ctype.h>
#include <string.h>
#include <setjmp.h>

typedef enum 
{
	EMP,//空
	NUM,//数
	SYM,//シンボル
	LIS,//リスト
	SUBR,//組み込み関数
}tag;

typedef enum 
{
	FRE,
	USE
}flag;


typedef struct 
{
	tag tag;//種類を記憶しておく場所
	flag flag;//GC(ガページコレクション)の時に使う目印
	char* name;//シンボルの名前を記憶する場所
	union {
		int num;//数アトムとして使われている場合のその数の値
		int bind;//参照する場合の参照する番地
		int (*subr)();//関数ポインタ
	}val;
	int car;//アトムが格納されているアドレス
	int cdr;//次の番地のアドレス
} cell;



cell heap[HEAPSIZE];
int hp;//heap pointer
int fc; //free counter


//-------read--------
#define EOL     '\n'
#define TAB     '\t'
#define SPACE   ' '
#define ESCAPE  033
#define NUL     '\0'



typedef enum { 
    LPAREN,// "("
    RPAREN,// ")"
    QUOTE, // "\"
    DOT,   // "."
    NUMBER,
    SYMBOL,
    OTHER
} toktype;


typedef enum {
    GO,
    BACK
} backtrack;

typedef struct {
    char ch;               //読み込んだ1文字
    backtrack flag;        //もう一回読むかどうかを判定
    toktype type;          //トークンの種類
    char buf[BUFSIZE];     //トークンの文字列
} token;

token stok = { GO,OTHER };

jmp_buf buf; //goto

void initcell(void);

//---------------------
int car(int addr);
int cdr(int addr);
int cons(int car, int cdr);
int caar(int addr);
int cdar(int addr);
int cadr(int addr);
int caddr(int addr);
//-------------------------


void gettoken(void);
int numbertoken(char buf[]);
int symboltoken(char buf[]);
int issymch(char c);

int freshcell(void);
int makenum(int num);
int makesym(char* name);

int read(void);
int readlist(void);

void print(int addr);
void printlist(int addr);
int listp(int addr);
int nullp(int addr);


void error(int errnum, char* fun, int arg);
int length(int addr);




int main(void) {
    printf("MiniLis Ver0.01\n");
    initcell();
    int ret = setjmp(buf);

repl:
    if (ret == 0) {
        while (1)
        {
            printf("> "); fflush(stdout); fflush(stdin);
            print(read());
            printf("\n"); fflush(stdout);
        }
    }
    else if (ret == 1) {
        ret = 0;
        goto repl;
    }
    else
    {
        return 0;
    }

}









//初期化・自由化リスト
void initcell(void) {
    int addr;

    for (addr = 0; addr < HEAPSIZE; addr++) {
        heap[addr].flag = FRE;
        heap[addr].cdr = addr + 1;
    }

    hp = 0;
    fc = HEAPSIZE;
}


//--------スキャナー---------------------
void gettoken(void) {
    char c;
    int pos;

    if (stok.flag == BACK) {
        stok.flag = GO;
        return;
    }

    if (stok.ch == ')') {
        stok.type = RPAREN;
        stok.ch = NUL;
        return;
    }

    if (stok.ch == '(') {
        stok.type = LPAREN;
        stok.ch = NUL;
        return;
    }

    c = getchar();
    while ((c == SPACE) || (c == EOL) || (c == TAB))
        c = getchar();

    switch (c) {
    case '(':   stok.type = LPAREN; break;
    case ')':   stok.type = RPAREN; break;
    case '\'':  stok.type = QUOTE; break;
    case '.':   stok.type = DOT; break;
    default: {
        pos = 0; stok.buf[pos++] = c;
        while (((c = getchar()) != EOL) && (pos < BUFSIZE) &&
            (c != SPACE) && (c != '(') && (c != ')'))
            stok.buf[pos++] = c;

        stok.buf[pos] = NUL;
        stok.ch = c;
        if (numbertoken(stok.buf)) {
            stok.type = NUMBER;
            break;
        }
        if (symboltoken(stok.buf)) {
            stok.type = SYMBOL;
            break;
        }
        stok.type = OTHER;
    }
    }
}

int numbertoken(char buf[]) {
    int i;
    char c;

    if ((buf[0] == '+') || (buf[0] == '-')) {
        if (buf[1] == NUL)
            return(0); // case {+,-} => symbol
        i = 1;
        while ((c = buf[i]) != NUL)
            if (isdigit(c))
                i++;  // case {+123..., -123...}
            else
                return(0);
    }
    else {
        i = 0;    // {1234...}
        while ((c = buf[i]) != NUL)
            if (isdigit(c))
                i++;
            else
                return(0);
    }
    return(1);
}

int symboltoken(char buf[]) {
    int i;
    char c;

    if (isdigit(buf[0]))
        return(0);

    i = 0;
    while ((c = buf[i]) != NUL)
        if ((isalpha(c)) || (isdigit(c)) || (issymch(c)))
            i++;
        else
            return(0);

    return(1);
}

int issymch(char c) {
    switch (c) {
    case '!':
    case '?':
    case '+':
    case '-':
    case '*':
    case '/':
    case '=':
    case '<':
    case '>': return(1);
    default:  return(0);
    }
}



//-----------------------

int freshcell(void) {
    int res;

    res = hp;
    hp = heap[hp].cdr;
    heap[res].cdr = 0;
    fc--;
    return(res);
}

//数アトムを生成
int makenum(int num) {
    int addr;

    addr = freshcell();
    heap[addr].tag = NUM;
    heap[addr].val.num = num;
    return(addr);
}

//シンボルアトムの生成
int makesym(char* name) {
    int addr;

    addr = freshcell();
    heap[addr].tag = SYM;
    heap[addr].name = (char*)malloc(SYMSIZE); 
    strcpy(heap[addr].name,name);
    return(addr);
}

//リスト操作
int car(int addr) {
    return(heap[addr].car);
}

int caar(int addr) {
    return(car(car(addr)));
}

int cdar(int addr) {
    return(cdr(car(addr)));
}

int cdr(int addr) {
    return(heap[addr].cdr);
}

int cadr(int addr) {
    return(car(cdr(addr)));
}

int caddr(int addr) {
    return(car(cdr(cdr(addr))));
}



//アトムをくっつける関数
int cons(int car, int cdr) {
    int addr;

    addr = freshcell();
    heap[addr].tag = LIS;
    heap[addr].car = car;
    heap[addr].cdr = cdr;
    return(addr);
}

int length(int addr) {
    int len = 0;

    while (!(nullp(addr))) {
        len++;
        addr = cdr(addr);
    }
    return(len);
}



//-------------------------------

int read(void) {
    gettoken();
    switch (stok.type) {
    case NUMBER:    return(makenum(atoi(stok.buf)));
    case SYMBOL:    return(makesym(stok.buf));
    case QUOTE:     return(cons(makesym("quote"), cons(read(), NIL)));
    case LPAREN:    return(readlist());
    default:        break;
    }
    error(CANT_READ_ERR, "read", NIL);
    return(0);
}

int readlist(void) {
    int car, cdr;

    gettoken();
    if (stok.type == RPAREN)
        return(NIL);
    else
        if (stok.type == DOT) {
            cdr = read();
            if (atomp(cdr))
                gettoken();
            return(cdr);
        }
        else {
            stok.flag = BACK;
            car = read();
            cdr = readlist();
            return(cons(car, cdr));
        }
}


//-----print------------------
void print(int addr) {
    switch (heap[addr].tag) {
    case NUM:   printf("%d", heap[addr].val.num); break;
    case SYM:   printf("%s", heap[addr].name); break;
    case SUBR:  printf("<subr>"); break;
    case LIS: { printf("(");
        printlist(addr); break; }
    default:    printf("<undef>"); break;
    }
}



void printlist(int addr) {
    if (addr == 0 || addr == 1)
        printf(")");
    else
        if ((!(listp(cdr(addr)))) && (!(nullp(cdr(addr))))) {
            print(car(addr));
            printf(" . ");
            print(cdr(addr));
            printf(")");
        }
        else {
            print(heap[addr].car);
            if (!(heap[addr].cdr == 0 || heap[addr].cdr == 1)) {
                printf(" ");
            }
            printlist(heap[addr].cdr);
        }
}

int atomp(int addr) {
    if (heap[addr].tag == NUM || heap[addr].tag == SYM)
        return(1);
    else
        return(0);
}

int numberp(int addr) {
    if (heap[addr].tag == NUM)
        return(1);
    else
        return(0);
}

int symbolp(int addr) {
    if (heap[addr].tag == SYM)
        return(1);
    else
        return(0);
}
//
int listp(int addr) {
    if (heap[addr].tag == LIS || addr == 0 || addr == 1)
        return(1);
    else
        return(0);
}

//
int nullp(int addr) {
    if (addr == 0 || addr == 1)
        return(1);
    else
        return(0);
}


//-------エラー処理------
void error(int errnum, char* fun, int arg) {
    switch (errnum) {
    case CANT_FIND_ERR: {printf("%s can't find definition of ", fun);
        print(arg); break; }

    case CANT_READ_ERR: {printf("%s can't read of ", fun);
        break; }

    case ILLEGAL_OBJ_ERR: {printf("%s got an illegal object ", fun);
        print(arg); break; }

    case ARG_SYM_ERR: {printf("%s require symbol but got ", fun);
        print(arg); break; }

    case ARG_NUM_ERR: {printf("%s require number but got ", fun);
        print(arg); break; }

    case ARG_LIS_ERR: {printf("%s require list but got ", fun);
        print(arg); break; }

    case ARG_LEN0_ERR: {printf("%s require 0 arg but got %d", fun, length(arg));
        break; }

    case ARG_LEN1_ERR: {printf("%s require 1 arg but got %d", fun, length(arg));
        break; }

    case ARG_LEN2_ERR: {printf("%s require 2 args but got %d", fun, length(arg));
        break; }

    case ARG_LEN3_ERR: {printf("%s require 3 args but got %d", fun, length(arg));
        break; }

    case MALFORM_ERR: {printf("%s got malformed args ", fun);
        print(arg); break; }

    case DIV_BY_ZERO: {printf("%s divide by zero ", fun);
        print(arg); break; }
    }
    printf("\n");
    longjmp(buf, 1);
}