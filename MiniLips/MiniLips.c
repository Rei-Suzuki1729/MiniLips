#include <stdio.h>
#include<stdlib.h>
#include <ctype.h>
#include <string.h>
#include <setjmp.h>
#include"MiniLips.h"

cell heap[HEAPSIZE];

token stok = { GO,OTHER };

jmp_buf buf; //goto

int stack[STACKSIZE];
int argstk[STACKSIZE];


int main(void) {
    printf("MiniLis Ver0.01\n");
    initcell();
    initsubr();
    int ret = setjmp(buf);

repl:
    if (ret == 0) {
        while (1)
        {
            printf("> "); fflush(stdout); fflush(stdin);
            print(eval(read()));
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

    //0番地はnil、環境レジスタを設定する。初期環境
    ep = makesym("nil");
    assocsym(makesym("nil"), NIL);
    assocsym(makesym("t"), makesym("t"));//ep = ((t.t)(nil.nil))という連想リスト

    sp = 0;
    ap = 0;
}

int freshcell(void) {
    int res;

    res = hp;
    hp = heap[hp].cdr;
    heap[res].cdr = 0;
    fc--;
    return(res);
}

//deep-bindによる。シンボルが見つからなかったら登録。
//見つかったらそこに値をいれておく。
void bindsym(int sym, int val) {
    int addr;

    addr = assoc(sym, ep);
    if (addr == 0)
        assocsym(sym, val);
    else
        heap[addr].cdr = val;
}


//変数の束縛
//ローカル変数の場合は以前の束縛に積み上げていく。
//したがって同じ変数名があったとしても書き変えない。
void assocsym(int sym, int val) {
    ep = cons(cons(sym, val), ep);
}


//環境は次のように連想リストになっている。
// env = ((sym1 . val1) (sym2 . val2) ...)
// assocでシンボルに対応する値を探す。
//見つからなかった場合には-1を返す。
int findsym(int sym) {
    int addr;

    addr = assoc(sym, ep);

    if (addr == 0)
        return(-1);
    else
        return(cdr(addr));
}


//-----------------------

//-------デバッグ用------------------    
void cellprint(int addr) {
    switch (heap[addr].flag) {
    case FRE:   printf("FRE "); break;
    case USE:   printf("USE "); break;
    }
    switch (heap[addr].tag) {
    case EMP:   printf("EMP    "); break;
    case NUM:   printf("NUM    "); break;
    case SYM:   printf("SYM    "); break;
    case LIS:   printf("LIS    "); break;
    case SUBR:  printf("SUBR   "); break;
    case FSUBR: printf("FSUBR  "); break;
    case FUNC:  printf("FUNC   "); break;
    }
    printf("%07d ", heap[addr].car);
    printf("%07d ", heap[addr].cdr);
    printf("%07d ", heap[addr].val.bind);
    printf("%s \n", heap[addr].name);
}

//ヒープダンプ  
void heapdump(int start, int end) {
    int i;

    printf("addr    F   TAG    CAR     CDR     BIND    NAME\n");
    for (i = start; i <= end; i++) {
        printf("%07d ", i);
        cellprint(i);
    }
}

//---------------------リスト操作-------------------------
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

int assoc(int sym, int lis) {
    if (nullp(lis))
        return(0);
    else if (eqp(sym, caar(lis)))
        return(car(lis));
    else
        return(assoc(sym, cdr(lis)));
}

int length(int addr) {
    int len = 0;

    while (!(nullp(addr))) {
        len++;
        addr = cdr(addr);
    }
    return(len);
}

int list(int arglist) {
    if (nullp(arglist))
        return(NIL);
    else
        return(cons(car(arglist), list(cdr(arglist))));
}
//--------------------------------------------


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

//スタック。ep環境ポインタの保存用
void push(int pt) {
    stack[sp++] = pt;
}

int pop(void) {
    return(stack[--sp]);
}

//arglistスタックのpush/pop
void argpush(int addr) {
    argstk[ap++] = addr;
}

void argpop(void) {
    --ap;
}


//------------read-----------------------------------------

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
//--------------------
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
//-----------------------------------------------------------

//-----print------------------------------------------------
void print(int addr) {
    switch (heap[addr].tag) {
    case NUM:   printf("%d", heap[addr].val.num); break;
    case SYM:   printf("%s", heap[addr].name); break;
    case SUBR:  printf("<subr>"); break;
    case FSUBR: printf("<fsubr>"); break;
    case FUNC:  printf("<function>"); break;
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
            if (!((heap[addr].cdr == 0) || (heap[addr].cdr == 1))) {
                printf(" ");
            }
            printlist(heap[addr].cdr);
        }
}
//----------------------------------------------------------


//-----------eval-------------------------------------------

int eval(int addr) {
    int res;

    //アトムの場合
    if (atomp(addr)) {
        //数の場合
        if (numberp(addr))
            return(addr);
        //シンボルの場合
        if (symbolp(addr)) {
            res = findsym(addr);
            if (res == -1)
                error(CANT_FIND_ERR, "eval", addr);
            else
                return(res);
        }
    }
    else//リストの場合
        if (listp(addr)) {
            if ((symbolp(car(addr))) && (strcmp(heap[car(addr)].name,"quote") == 0))
                return(cadr(addr));
            if (numberp(car(addr)))
                error(ARG_SYM_ERR, "eval", addr);
            if (subrp(car(addr)))
                return(apply(car(addr), evlis(cdr(addr))));
            if (fsubrp(car(addr)))
                return(apply(car(addr), cdr(addr)));
            if (functionp(car(addr)))
                return(apply(car(addr), evlis(cdr(addr))));
        }
    error(CANT_FIND_ERR, "eval", addr);
    return(0);
}

int apply(int func, int args) {
    int symaddr, varlist, body, res;

    symaddr = findsym(func); //組み込み関数の場合はdefsubrで定義している
    if (symaddr == -1)
        error(CANT_FIND_ERR, "apply", func);
    else {
        switch (heap[symaddr].tag) {
        case SUBR:  return((heap[symaddr].val.subr)(args));
        case FSUBR: return((heap[symaddr].val.subr)(args));
        case FUNC: {    varlist = car(heap[symaddr].val.bind);
            body = cdr(heap[symaddr].val.bind);
            bindarg(varlist, args);
            while (!((body == 0) || (body == 1))) {
                res = eval(car(body));
                body = cdr(body);
            }
            unbind();
            return(res); }
        default:    error(ILLEGAL_OBJ_ERR, "eval", symaddr);
        }
    }
    return(0);
}

void bindarg(int varlist, int arglist) {
    int arg1, arg2;

    push(ep);
    while (!((varlist == 0) || (varlist == 1))) {
        arg1 = car(varlist);
        arg2 = car(arglist);
        assocsym(arg1, arg2);
        varlist = cdr(varlist);
        arglist = cdr(arglist);
    }
}

void unbind(void) {
    ep = pop();
}

//変数を束縛されている値に置き換える
int evlis(int addr) {
    int car_addr, cdr_addr;

    argpush(addr);
    if ((addr ==0) ||(addr ==1)) {
        argpop();
        return(addr);
    }
    else {
        car_addr = eval(car(addr));
        argpush(car_addr);
        cdr_addr = evlis(cdr(addr));
        argpop();
        argpop();
        return(cons(car_addr, cdr_addr));
    }
}

//アトムかどうかタグで判定
int atomp(int addr) {
    if ((heap[addr].tag == NUM) || (heap[addr].tag == SYM))
        return(1);
    else
        return(0);
}
//数かどうかタグで判定
int numberp(int addr) {
    if (heap[addr].tag == NUM)
        return(1);
    else
        return(0);
}
//シンボルかどうかタグで判定
int symbolp(int addr) {
    if (heap[addr].tag == SYM)
        return(1);
    else
        return(0);
}
//リストかどうかタグで判定
int listp(int addr) {
    if ((heap[addr].tag == LIS) || (addr == 0) || (addr == 1))
        return(1);
    else
        return(0);
}
//番地が0か1の判定場合
int nullp(int addr) {
    if ((addr == 0) || (addr == 1))
        return(1);
    else
        return(0);
}

//同じかどうか判定
int eqp(int addr1, int addr2) {
    if ((numberp(addr1)) && (numberp(addr2))
        && ((heap[addr1].val.num) == (heap[addr2].val.num)))
        return(1);
    else if ((symbolp(addr1)) && (symbolp(addr2))
        && (strcmp(heap[addr1].name,heap[addr2].name) == 0))
        return(1);
    else
        return(0);
}

//組み込み関数かどうかタグで判定
int subrp(int addr) {
    int val;

    val = findsym(addr);
    if (val != -1)
        return(heap[addr].tag == SUBR);
    else
        return(0);
}

int fsubrp(int addr) {
    int val;

    val = findsym(addr);
    if (val != -1)
        return(heap[val].tag == FSUBR);
    else
        return(0);
}

int functionp(int addr) {
    int val;

    val = findsym(addr);
    if (val != -1)
        return(heap[val].tag == FUNC);
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



//--------組込み関数---------------------------------
//subrを環境に登録する。
void defsubr(char* symname, int(*func)(int)) {
    bindfunc(symname, SUBR, func);
}

//fsubr(引数を評価しない組込関数）の登録。
void deffsubr(char* symname, int(*func)(int)) {
    bindfunc(symname, FSUBR, func);
}

void bindfunc(char* name, tag tag, int(*func)(int)) {
    int sym, val;

    sym = makesym(name);
    val = freshcell();
    heap[val].tag = tag;
    heap[val].val.subr = func;
    heap[val].cdr = 0;
    bindsym(sym, val);
}

void bindfunc1(char* name, int addr) {
    int sym, val;

    sym = makesym(name);
    val = freshcell();
    heap[val].tag = FUNC;
    heap[val].val.bind = addr;
    heap[val].cdr = 0;
    bindsym(sym, val);
}

void initsubr(void) {
    defsubr("+", f_plus);
    defsubr("-", f_minus);
    defsubr("*", f_mult);
    defsubr("/", f_div);
    defsubr("exit", f_exit);
    defsubr("quit", f_exit);
    defsubr("hdmp", f_heapdump);
    defsubr("car", f_car);
    defsubr("cdr", f_cdr);
    defsubr("cons", f_cons);
    defsubr("list", f_list);
    defsubr("eq", f_eq);
    defsubr("null", f_nullp);
    defsubr("atom", f_atomp);
    defsubr("oblist", f_oblist);

    defsubr("read", f_read);
    defsubr("eval", f_eval);
    defsubr("apply", f_apply);
    defsubr("print", f_print);
    defsubr("=", f_numeqp);
    defsubr(">", f_greater);
    defsubr(">=", f_eqgreater);
    defsubr("<", f_smaller);
    defsubr("<=", f_eqsmaller);
    defsubr("numberp", f_numberp);
    defsubr("symbolp", f_symbolp);
    defsubr("listp", f_listp);

    deffsubr("setq", f_setq);
    deffsubr("defun", f_defun);
    deffsubr("if", f_if);
    deffsubr("begin", f_begin);
    deffsubr("cond", f_cond);
}
   

//-----組み込み関数定義---------------------

int f_plus(int arglist) {
    int arg, res;

    res = 0;
    while (!((arglist == 0) || (arglist == 1))) {
        arg = heap[car(arglist)].val.num;
        arglist = cdr(arglist);
        res = res + arg;
    }
    return(makenum(res));
}

int f_minus(int arglist) {
    int arg, res;

    res = heap[car(arglist)].val.num;
    arglist = cdr(arglist);
    while (!((arglist == 0) || (arglist == 1))) {
        arg = heap[car(arglist)].val.num;
        arglist = cdr(arglist);
        res = res - arg;
    }
    return(makenum(res));
}

int f_mult(int arglist) {
    int arg, res;

    res = heap[car(arglist)].val.num;
    arglist = cdr(arglist);
    while (!((arglist == 0) || (arglist == 1))) {
        arg = heap[car(arglist)].val.num;
        arglist = cdr(arglist);
        res = res * arg;
    }
    return(makenum(res));
}

int f_div(int arglist) {
    int arg, res;

    res = heap[car(arglist)].val.num;
    arglist = cdr(arglist);
    while (!((arglist == 0) || (arglist == 1))) {
        arg = heap[car(arglist)].val.num;
        if (arg == 0)
            error(DIV_BY_ZERO, "/", arglist);
        arglist = cdr(arglist);
        res = res / arg;
    }
    return(makenum(res));
}

int f_exit(int arglist) {
    int addr;

    for (addr = 0; addr < HEAPSIZE; addr++)
        free(heap[addr].name);

    printf("- good bye -\n");
    longjmp(buf, 2);
}

int f_heapdump(int arglist) {
    int arg1;

    arg1 = heap[car(arglist)].val.num;
    heapdump(arg1, arg1 + 10);
    return(T);
}

int f_car(int arglist) {
    int arg1;

    arg1 = car(arglist);
    return(car(arg1));
}

int f_cdr(int arglist) {
    int arg1;

    arg1 = car(arglist);
    return(cdr(arg1));
}

int f_cons(int arglist) {
    int arg1, arg2;

    arg1 = car(arglist);
    arg2 = cadr(arglist);
    return(cons(arg1, arg2));
}

int f_list(int arglist) {
    return(list(arglist));
}

int f_eq(int arglist) {
    int arg1, arg2;

    arg1 = car(arglist);
    arg2 = cadr(arglist);
    if (eqp(arg1, arg2))
        return(T);
    else
        return(NIL);
}

int f_nullp(int arglist) {
    int arg;

    arg = car(arglist);
    if (nullp(arg))
        return(T);
    else
        return(NIL);
}

int f_atomp(int arglist) {
    int arg;

    arg = car(arglist);
    if (atomp(arg))
        return(T);
    else
        return(NIL);
}

int f_length(int arglist) {
    return(makenum(length(car(arglist))));
}

int f_numeqp(int arglist) {
    int num1, num2;

    num1 = heap[car(arglist)].val.num;
    num2 = heap[cadr(arglist)].val.num;

    if (num1 == num2)
        return(T);
    else
        return(NIL);
}

int f_symbolp(int arglist) {
    if (symbolp(car(arglist)))
        return(T);
    else
        return(NIL);
}

int f_numberp(int arglist) {
    if (numberp(car(arglist)))
        return(T);
    else
        return(NIL);
}

int f_listp(int arglist) {
    if (listp(car(arglist)))
        return(T);
    else
        return(NIL);
}

int f_smaller(int arglist) {
    int num1, num2;

    num1 = heap[car(arglist)].val.num;
    num2 = heap[cadr(arglist)].val.num;

    if (num1 < num2)
        return(T);
    else
        return(NIL);
}

int f_eqsmaller(int arglist) {
    int num1, num2;

    num1 = heap[car(arglist)].val.num;
    num2 = heap[cadr(arglist)].val.num;

    if (num1 <= num2)
        return(T);
    else
        return(NIL);
}

int f_greater(int arglist) {
    int num1, num2;

    num1 = heap[car(arglist)].val.num;
    num2 = heap[cadr(arglist)].val.num;

    if (num1 > num2)
        return(T);
    else
        return(NIL);
}

int f_eqgreater(int arglist) {
    int num1, num2;

    num1 = heap[car(arglist)].val.num;
    num2 = heap[cadr(arglist)].val.num;

    if (num1 >= num2)
        return(T);
    else
        return(NIL);
}

int f_oblist(int arglist) {
    int addr, addr1, res;

    res = NIL;
    addr = ep;
    while (!(nullp(addr))) {
        addr1 = caar(addr);
        res = cons(addr1, res);
        addr = cdr(addr);
    }
    return(res);
}

int f_read(int arglist) {
    return(read());
}

int f_print(int arglist) {
    print(car(arglist));
    printf("\n");
    return(T);
}


int f_eval(int arglist) {
    return(eval(car(arglist)));
}

int f_apply(int arglist) {
    int arg1, arg2;

    arg1 = car(arglist);
    arg2 = cadr(arglist);
    return(apply(arg1, arg2));
}


//--FSUBR-----------
int f_setq(int arglist) {
    int arg1, arg2;

    arg1 = car(arglist);
    arg2 = eval(cadr(arglist));
    bindsym(arg1, arg2);
    return(T);
}

int f_defun(int arglist) {
    int arg1, arg2;

    arg1 = car(arglist);
    arg2 = cdr(arglist);
    bindfunc1(heap[arg1].name, arg2);
    return(T);
}

int f_if(int arglist) {
    int arg1, arg2, arg3;

    arg1 = car(arglist);
    arg2 = cadr(arglist);
    arg3 = car(cdr(cdr(arglist)));

    if (!(nullp(eval(arg1))))
        return(eval(arg2));
    else
        return(eval(arg3));
}

int f_cond(int arglist) {
    int arg1, arg2, arg3;

    if (nullp(arglist))
        return(NIL);

    arg1 = car(arglist);
    arg2 = car(arg1);
    arg3 = cdr(arg1);

    if (!(nullp(eval(arg2))))
        return(f_begin(arg3));
    else
        return(f_cond(cdr(arglist)));
}

int f_begin(int arglist) {
    int res;

    while (!(nullp(arglist))) {
        res = eval(car(arglist));
        arglist = cdr(arglist);
    }
    return(res);
}
