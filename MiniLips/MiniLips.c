#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

enum tag
{
	EMP,//空
	NUM,//数
	SYM,//シンボル
};

typedef tag tag;



struct cell
{
	tag tag;//種類を記憶しておく場所
	char* name;//シンボルの名前を記憶する場所
	union {
		int num;//数アトムとして使われている場合のその数の値
	};
	int car;//アトムが格納されているアドレス
	int cdr;//次の番地のアドレス
};

typedef struct cell cell;



int main(void) {
	


}
