#define HEAPSIZE 10000000
#define NIL 0
#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>

enum tag
{
	EMP,//��
	NUM,//��
	SYM,//�V���{��
	LIS,//���X�g
	SUBR,//�g�ݍ��݊֐�
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
};

typedef struct cell cell;

cell heap[HEAPSIZE];
int hp;//heap pointer

//������
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
