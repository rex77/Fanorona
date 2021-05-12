#pragma once //소스파일 중복 로드 방지
#include "main.h"
#include "user.h"

//#define DEBUG 1

typedef struct dialog {
	char *question;
	char *answer[3];
	char *reaction[3];
	int score[3];
} DIALOG;

//타입 정의
typedef int BOARD; //보드
typedef enum Pawn { Black = -1, Empty = 0, White = 1 } PAWN;//말의 색상을 부호만 다르게 둠으로써 턴을 교환할 때 * -1 로 간편하게 전환 가능함
typedef enum State { Black_win = -1, No_win = 0, White_win = 1, Draw = 2 } STATE; //보드판의 상태
typedef enum Direction { North = -9, Northwest = -10, West = -1, Southwest = 8, South = 9, Southeast = 10, East = 1, Northeast = -8 } DIRECTION; //이동 가능한 방향
typedef enum Dialog_state { In_progress, Failed, Successed, Nomore } DIALOG_STATE;
ALLEGRO_TIMER *turn_timer; //턴의 제한시간을 계산할 때 필요한 타이머

void init_board(BOARD board[]); //보드 초기화
void play_game(int mode); //게임에 필요한 설정 초기화 && 실제 게임 루틴
STATE check_board(BOARD board[]); //보드 검사

int whos_first(void); //플레이어의 선을 정함. 항상 하얀색이 선공이다
int set_cpu_level(void); //cpu 레벨 변경
int player_move(int color, USER *player, BOARD board[]); //플레이어가 말을 이동하는 함수
int computer_move(int color, USER *cpu, int level, BOARD board[]); //cpu가 말을 이동하는 함수
int random_select(int available[]);

bool is_moveable(int src, int dest, DIRECTION dir, BOARD board[]); //src에서 dest 위치로 이동할 수 있는가
bool is_catchable(int src, int dest, DIRECTION dir, BOARD board[]); //src에서 dir방향의 dest 위치에 있는 말을 삭제할 수 있는가
bool is_first_time(int position, int trail[]); //연속 이동을 할 때 지나온 위치가 아닌가

int calculate_pos(int cur_pos, int color, DIRECTION dir, int board[]); //점수를 내놓아라
int calculate_pawn(int pos, int color, int depth, int *trail, BOARD ori_board[], int *movement); //위치를 이용해 계산하기
void set_dialog(DIALOG dialog[], int lv);

int campaign_chat(int lv);