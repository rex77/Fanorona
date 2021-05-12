//어디서든 쓰이는 함수 및 변수
//파노로나 게임이 아니라 알레그로 전반에 사용됨
#pragma once
#include <stdio.h> 
#include <allegro5/allegro.h> //알레그로 라이브러리
#include <allegro5/allegro_native_dialog.h> //알레그로 네이티브 창 관련 애드온

#define FPS 30 //타이머에 사용되는 FPS
#define EOA 45 //End of Array, 문자열의 '\0'과 같이 배열에 들어있는 유효한 좌표의 끝을 나타낸다.
			   //배열은 주로 보드 상의 좌표를 저장하기 위해 사용되는데, 이미 0이 첫번째 칸을 나타내기 때문에 오인될 소지가 커서 45로 설정
enum Menu { Campaign, Freeplay, Two_player, Tutorial, Option, Profile, Exit }; //타이틀의 메뉴 번호와 같게 해야함

ALLEGRO_TIMER *timer; //알레그로 타이머
ALLEGRO_EVENT_QUEUE *queue; //알레그로 이벤트 큐
ALLEGRO_EVENT event; //알레그로 이벤트 저장 변수, 이벤트 큐에서 이벤트를 POP해서 임시로 저장하는 역할

void must_init(bool func, const char *str); //개체 생성 확인용 검사 함수
void show_tutorial(void);
void show_options(void);
int show_title(void);
void show_profile(void);