#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

enum tag
{
	EMP,//��
	NUM,//��
	SYM,//�V���{��
};

typedef tag tag;



struct cell
{
	tag tag;//��ނ��L�����Ă����ꏊ
	char* name;//�V���{���̖��O���L������ꏊ
	union {
		int num;//���A�g���Ƃ��Ďg���Ă���ꍇ�̂��̐��̒l
	};
	int car;//�A�g�����i�[����Ă���A�h���X
	int cdr;//���̔Ԓn�̃A�h���X
};

typedef struct cell cell;



int main(void) {
	


}
