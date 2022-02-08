#define HEAPSIZE 10000000
#define BUFSIZE 256
#define SYMSIZE 256
#define NIL 0
#define _CRT_SECURE_NO_WARNINGS





#include <stdio.h>
#include <ctype.h>
#include <string.h>


typedef enum 
{
	EMP,//��
	NUM,//��
	SYM,//�V���{��
	LIS,//���X�g
	SUBR,//�g�ݍ��݊֐�
}tag;

typedef enum 
{
	FRE,
	USE
}flag;


typedef struct 
{
	tag tag;//��ނ��L�����Ă����ꏊ
	flag flag;//GC(�K�y�[�W�R���N�V����)�̎��Ɏg���ڈ�
	char* name;//�V���{���̖��O���L������ꏊ
	union {
		int num;//���A�g���Ƃ��Ďg���Ă���ꍇ�̂��̐��̒l
		int bind;//�Q�Ƃ���ꍇ�̎Q�Ƃ���Ԓn
		int (*subr)();//�֐��|�C���^
	}val;
	int car;//�A�g�����i�[����Ă���A�h���X
	int cdr;//���̔Ԓn�̃A�h���X
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
    char ch;               //�ǂݍ���1����
    backtrack flag;        //�������ǂނ��ǂ����𔻒�
    toktype type;          //�g�[�N���̎��
    char buf[BUFSIZE];     //�g�[�N���̕�����
} token;

token stok = { GO,OTHER };



//�������E���R�����X�g
void initcell(void) {
    int addr;

    for (addr = 0; addr < HEAPSIZE; addr++) {
        heap[addr].flag = FRE;
        heap[addr].cdr = addr + 1;
    }

    hp = 0;
    fc = HEAPSIZE;
}


//scanner
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

//���A�g���𐶐�
int makenum(int num) {
    int addr;

    addr = freshcell();
    heap[addr].tag = NUM;
    heap[addr].val.num = num;
    return(addr);
}

//�V���{���A�g���̐���
int makesym(char* name) {
    int addr;

    addr = freshcell();
    heap[addr].tag = SYM;
    heap[addr].name = (char*)malloc(SYMSIZE); 
    strcpy(heap[addr].name,name);
    return(addr);
}

//�A�g������������֐�
int cons(int car, int cdr) {
    int addr;

    addr = freshcell();
    heap[addr].tag = LIS;
    heap[addr].car = car;
    heap[addr].cdr = cdr;
    return(addr);
}
//-------------------------------



int main(void) {
    gettoken();
    printf("%s", stok.buf);

}
