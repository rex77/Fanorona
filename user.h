#pragma once
#include <allegro5/allegro.h>

typedef struct user { //오프라인용 유저 구조체, db에 업로드하지 않는 이상은 게임을 끝낼때까지만 이 데이터 사용
	char name[20];
	ALLEGRO_BITMAP *toon; //상단 화면에 표시될 사용자의 캐릭터 사진, 1750 x 300픽셀의 스프라이트 시트 형태로 받음
	int win; //전적 (승리)
	int tie; //전적 (무승부)
	int lose; //전적 (패배)
} USER;


USER local_user; //지금 게임을 실행하고 있는 플레이어
USER guest; //2p 모드에서 등장하는 게스트 플레이어
USER cpu; //cpu

USER *p1; //실제 플레이어를 가리키는 USER형 포인터, 게임 관련 함수에서 플레이어 구조체의 이름을 바로 받을 수 없으니 일관성을 위해서 사용
USER *p2; //실제 플레이어를 가리키는 USER형 포인터, 게임 관련 함수에서 플레이어 구조체의 이름을 바로 받을 수 없으니 일관성을 위해서 사용

char logined_id[20];

bool login; //현재 로그인 여부

void init_user(void); //로컬 유저 초기화
void init_guest(void); //게스트 초기화
void init_cpu(int lv); //cpu 플레이어 정보를 초기화
bool login_or_register(void);
bool login_user(void);
bool register_user(void);
void update_user_result(void);
void update_user_toon(char *path);
void update_user_name(char *name);
void backslash_to_slash(char *str);