# MiniLips
 
MiniLis�Ƃ�Lips�Ƃ����v���O���~���O����̏k���łł�
��̓I�ɂ����΁A

- �����݈̂����܂�
- �l�����Z���ł��܂�
- ���l��r�ł��܂�(�^�U����)
- �ϐ��A�֐����`�A�v�Z�ł��܂�
 
# DEMO
���̂܂ܕԂ��Ă��܂�
```
> 1
1
```
�O�u�L�@�Ŏl�����Z�������܂�
```
> (+ 1 2)
3
```
�ϐ��ɒl������(��������)
```
> (setq a 12)
t
>a
12
```
��ɂ���a���V���{���Ƃ����܂�
�V���{���������Ƒ������ꂽ�l���Ԃ��Ă��܂�
�V���{���ɃN�I�[�g�L����t����ƃV���{�����̂��̂��Ԃ��Ă��܂�
```
> 'a
a
```
t�͐^�U�̐^��\���܂��B�܂�nil�͋U��\���Ă��܂��B
```
> (> 2 1)
t
>(> 1 2)
nil
```
�֐����`�B���̎��͌v�Z���ꂽ���ʂ��Ԃ��Ă��܂��B
```
> (defun f(x) (+ x 10))
t
>(f 2)
12
```
��������B��͊K����ċA�Ōv�Z���Ă���Ƃ���ł�
```
> (defun fact (n) (if (= n 0) 1(* n (fact (- n 1)))))
t
> (fact 3)
6
>
```
# Usage
## visual studio�̏ꍇ
```
git clone https://github.com/Rei-Suzuki1729/MiniLips.git
```
MiniLips.sln���N���b�N����ƃ\�����[�V�����G�N�X�v���[���[���J��
���̏�ԂŃf�o�b�O����B
## ����ȊO�̏ꍇ
```
git clone https://github.com/Rei-Suzuki1729/MiniLips.git
cd MiniLips/MiniLips
gcc MiniLips.c -o MiniLips
```
> ~/MiniLips/MiniLips�Ƃ����p�X�̒���c�t�@�C���ƃw�b�_�[�t�@�C��������܂�

# Author
[Rei-Suzuki1729](https://github.com/Rei-Suzuki1729)