#pragma once
#include <allegro5/allegro.h>

typedef struct user { //�������ο� ���� ����ü, db�� ���ε����� �ʴ� �̻��� ������ ������������ �� ������ ���
	char name[20];
	ALLEGRO_BITMAP *toon; //��� ȭ�鿡 ǥ�õ� ������� ĳ���� ����, 1750 x 300�ȼ��� ��������Ʈ ��Ʈ ���·� ����
	int win; //���� (�¸�)
	int tie; //���� (���º�)
	int lose; //���� (�й�)
} USER;


USER local_user; //���� ������ �����ϰ� �ִ� �÷��̾�
USER guest; //2p ��忡�� �����ϴ� �Խ�Ʈ �÷��̾�
USER cpu; //cpu

USER *p1; //���� �÷��̾ ����Ű�� USER�� ������, ���� ���� �Լ����� �÷��̾� ����ü�� �̸��� �ٷ� ���� �� ������ �ϰ����� ���ؼ� ���
USER *p2; //���� �÷��̾ ����Ű�� USER�� ������, ���� ���� �Լ����� �÷��̾� ����ü�� �̸��� �ٷ� ���� �� ������ �ϰ����� ���ؼ� ���

char logined_id[20];

bool login; //���� �α��� ����

void init_user(void); //���� ���� �ʱ�ȭ
void init_guest(void); //�Խ�Ʈ �ʱ�ȭ
void init_cpu(int lv); //cpu �÷��̾� ������ �ʱ�ȭ
bool login_or_register(void);
bool login_user(void);
bool register_user(void);
void update_user_result(void);
void update_user_toon(char *path);
void update_user_name(char *name);
void backslash_to_slash(char *str);