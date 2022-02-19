#define HEAPSIZE 10000000
#define FREESIZE       50
#define STACKSIZE  30000
#define SYMSIZE 256
#define BUFSIZE 256
#define NIL 0
#define T 4



typedef enum
{
	EMP,//空
	NUM,//数
	SYM,//シンボル
	LIS,//リスト
	SUBR,//組み込み関数
	FSUBR,//特殊な組み込み関数
	FUNC
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
		int (*subr) ();//関数ポインタ
	}val;
	int car;//アトムが格納されているアドレス
	int cdr;//次の番地のアドレス
} cell;

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




//------pointer----
int ep; //environment pointer
int hp; //heap pointer 
int sp; //stack pointer
int fc; //free counter
int ap; //arglist pointer


#define GET_CAR(addr)       heap[addr].car
#define GET_CDR(addr)       heap[addr].cdr
#define GET_NUMBER(addr)    heap[addr].val.num
#define GET_NAME(addr)      heap[addr].name
#define GET_TAG(addr)       heap[addr].tag
#define GET_BIND(addr)      heap[addr].val.bind
#define GET_SUBR(addr)      heap[addr].val.subr
#define GET_FLAG(addr)      heap[addr].flag
#define SET_TAG(addr,x)     heap[addr].tag = x
#define SET_CAR(addr,x)     heap[addr].car = x
#define SET_CDR(addr,x)     heap[addr].cdr = x
#define SET_NUMBER(addr,x)  heap[addr].val.num = x
#define SET_BIND(addr,x)    heap[addr].val.bind = x
#define SET_NAME(addr,x)    heap[addr].name = (char *)malloc(SYMSIZE); strcpy(heap[addr].name,x)
#define SET_SUBR(addr,x)    heap[addr].val.subr = x
#define IS_SYMBOL(addr)     heap[addr].tag == SYM
#define IS_NUMBER(addr)     heap[addr].tag == NUM
#define IS_LIST(addr)       heap[addr].tag == LIS
#define IS_NIL(addr)        (addr == 0 || addr == 1)
#define IS_SUBR(addr)       heap[addr].tag == SUBR
#define IS_FSUBR(addr)      heap[addr].tag == FSUBR
#define IS_FUNC(addr)       heap[addr].tag == FUNC
#define IS_EMPTY(addr)      heap[addr].tag  == EMP
#define HAS_NAME(addr,x)    strcmp(heap[addr].name,x) == 0
#define SAME_NAME(addr1,addr2) strcmp(heap[addr1].name, heap[addr2].name) == 0
#define EQUAL_STR(x,y)      strcmp(x,y) == 0
#define MARK_CELL(addr)     heap[addr].flag = USE
#define NOMARK_CELL(addr)   heap[addr].flag = FRE
#define USED_CELL(addr)     heap[addr].flag == USE
#define FREE_CELL(addr)     heap[addr].flag == FRE


//-------read--------
#define EOL     '\n'
#define TAB     '\t'
#define SPACE   ' '
#define ESCAPE  033
#define NUL     '\0'

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

//-------arg check code--
#define NUMLIST_TEST    1
#define SYMBOL_TEST     2
#define NUMBER_TEST     3
#define LIST_TEST       4
#define LEN0_TEST       5
#define LEN1_TEST       6
#define LEN2_TEST       7
#define LEN3_TEST       8
#define LENS1_TEST      9
#define LENS2_TEST      10
#define COND_TEST       11  

//--------------------------
void initcell(void);
int freshcell(void);
void bindsym(int sym, int val);
void assocsym(int sym, int val);
int findsym(int sym);
void cellprint(int addr);
void heapdump(int start, int end);
void markoblist(void);
void markcell(int addr);
void gbcmark(void);
void gbcsweep(void);
void clrcell(int addr);
void gbc(void);
void checkgbc(void);
//---------------------
int car(int addr);
int cdr(int addr);
int cons(int car, int cdr);
int caar(int addr);
int cdar(int addr);
int cadr(int addr);
int caddr(int addr);
int assoc(int sym, int lis);
int length(int addr);
int list(int addr);
//-------------------------

int makenum(int num);
int makesym(char* name);
//----------------------------
void gettoken(void);
int numbertoken(char buf[]);
int symboltoken(char buf[]);
int issymch(char c);
int read(void);
int readlist(void);
//-----------------------------


//-------------------------------
void print(int addr);
void printlist(int addr);
//-------------------------------

//-------------------------------------
int eval(int addr);
void bindarg(int lambda, int arglist);
void unbind(void);
int atomp(int addr);
int numberp(int addr);
int symbolp(int addr);
int listp(int addr);
int nullp(int addr);
int eqp(int addr1, int addr2);
int evlis(int addr);
int apply(int func, int arg);
int subrp(int addr);
int fsubrp(int addr);
int functionp(int addr);
void initsubr(void);
void defsubr(char* symname, int(*func)(int));
void deffsubr(char* symname, int(*func)(int));
void bindfunc(char* name, tag tag, int(*func)(int));
void bindfunc1(char* name, int addr);
void push(int pt);
int pop(void);
void argpush(int addr);
void argpop(void);
void error(int errnum, char* fun, int arg);
void checkarg(int test, char* fun, int arg);
int isnumlis(int arg);

//---subr-------
int f_plus(int arglist);
int f_minus(int arglist);
int f_mult(int arglist);
int f_div(int arglist);
int f_exit(int arglist);
int f_heapdump(int arglist);
int f_car(int arglist);
int f_cdr(int arglist);
int f_cons(int arglist);
int f_length(int arglist);
int f_list(int arglist);
int f_nullp(int arglist);
int f_atomp(int arglist);
int f_eq(int arglist);
int f_setq(int arglist);
int f_oblist(int arglist);
int f_defun(int arglist);
int f_if(int arglist);
int f_cond(int arglist);
int f_numeqp(int arglist);
int f_numberp(int arglist);
int f_symbolp(int arglist);
int f_listp(int arglist);
int f_greater(int arglist);
int f_eqgreater(int arglist);
int f_smaller(int arglist);
int f_eqsmaller(int arglist);
int f_gbc(int arglist);
int f_eval(int arglist);
int f_apply(int arglist);
int f_read(int arglist);
int f_print(int arglist);
int f_begin(int arglist);

