#pragma once //�ҽ����� �ߺ� �ε� ����
#include "main.h"
#include "user.h"

//#define DEBUG 1

typedef struct dialog {
	char *question;
	char *answer[3];
	char *reaction[3];
	int score[3];
} DIALOG;

//Ÿ�� ����
typedef int BOARD; //����
typedef enum Pawn { Black = -1, Empty = 0, White = 1 } PAWN;//���� ������ ��ȣ�� �ٸ��� �����ν� ���� ��ȯ�� �� * -1 �� �����ϰ� ��ȯ ������
typedef enum State { Black_win = -1, No_win = 0, White_win = 1, Draw = 2 } STATE; //�������� ����
typedef enum Direction { North = -9, Northwest = -10, West = -1, Southwest = 8, South = 9, Southeast = 10, East = 1, Northeast = -8 } DIRECTION; //�̵� ������ ����
typedef enum Dialog_state { In_progress, Failed, Successed, Nomore } DIALOG_STATE;
ALLEGRO_TIMER *turn_timer; //���� ���ѽð��� ����� �� �ʿ��� Ÿ�̸�

void init_board(BOARD board[]); //���� �ʱ�ȭ
void play_game(int mode); //���ӿ� �ʿ��� ���� �ʱ�ȭ && ���� ���� ��ƾ
STATE check_board(BOARD board[]); //���� �˻�

int whos_first(void); //�÷��̾��� ���� ����. �׻� �Ͼ���� �����̴�
int set_cpu_level(void); //cpu ���� ����
int player_move(int color, USER *player, BOARD board[]); //�÷��̾ ���� �̵��ϴ� �Լ�
int computer_move(int color, USER *cpu, int level, BOARD board[]); //cpu�� ���� �̵��ϴ� �Լ�
int random_select(int available[]);

bool is_moveable(int src, int dest, DIRECTION dir, BOARD board[]); //src���� dest ��ġ�� �̵��� �� �ִ°�
bool is_catchable(int src, int dest, DIRECTION dir, BOARD board[]); //src���� dir������ dest ��ġ�� �ִ� ���� ������ �� �ִ°�
bool is_first_time(int position, int trail[]); //���� �̵��� �� �� ������ ��ġ�� �ƴѰ�

int calculate_pos(int cur_pos, int color, DIRECTION dir, int board[]); //������ �����ƶ�
int calculate_pawn(int pos, int color, int depth, int *trail, BOARD ori_board[], int *movement); //��ġ�� �̿��� ����ϱ�
void set_dialog(DIALOG dialog[], int lv);

int campaign_chat(int lv);