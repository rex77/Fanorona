//��𼭵� ���̴� �Լ� �� ����
//�ĳ�γ� ������ �ƴ϶� �˷��׷� ���ݿ� ����
#pragma once
#include <stdio.h> 
#include <allegro5/allegro.h> //�˷��׷� ���̺귯��
#include <allegro5/allegro_native_dialog.h> //�˷��׷� ����Ƽ�� â ���� �ֵ��

#define FPS 30 //Ÿ�̸ӿ� ���Ǵ� FPS
#define EOA 45 //End of Array, ���ڿ��� '\0'�� ���� �迭�� ����ִ� ��ȿ�� ��ǥ�� ���� ��Ÿ����.
			   //�迭�� �ַ� ���� ���� ��ǥ�� �����ϱ� ���� ���Ǵµ�, �̹� 0�� ù��° ĭ�� ��Ÿ���� ������ ���ε� ������ Ŀ�� 45�� ����
enum Menu { Campaign, Freeplay, Two_player, Tutorial, Option, Profile, Exit }; //Ÿ��Ʋ�� �޴� ��ȣ�� ���� �ؾ���

ALLEGRO_TIMER *timer; //�˷��׷� Ÿ�̸�
ALLEGRO_EVENT_QUEUE *queue; //�˷��׷� �̺�Ʈ ť
ALLEGRO_EVENT event; //�˷��׷� �̺�Ʈ ���� ����, �̺�Ʈ ť���� �̺�Ʈ�� POP�ؼ� �ӽ÷� �����ϴ� ����

void must_init(bool func, const char *str); //��ü ���� Ȯ�ο� �˻� �Լ�
void show_tutorial(void);
void show_options(void);
int show_title(void);
void show_profile(void);