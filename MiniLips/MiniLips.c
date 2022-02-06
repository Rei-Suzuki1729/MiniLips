#define HEAPSIZE 10000000
#define NIL 0
#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>

enum tag
{
	EMP,//空
	NUM,//数
	SYM,//シンボル
	LIS,//リスト
	SUBR,//組み込み関数
};

typedef tag tag;

enum flag
{
	FRE,
	USE
};

typedef flag flag;

struct cell
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
};

typedef struct cell cell;

cell heap[HEAPSIZE];
int hp;//heap pointer

//初期化
void initcell(void) {
	int addr;

	for (addr = 0; addr < HEAPSIZE; addr++) {
		heap[addr].flag = FRE;
		heap[addr].cdr = addr + 1;
	}

	hp = 0;
}


int main(void) {
	


}
