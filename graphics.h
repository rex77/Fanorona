#pragma once
#include "main.h"
#include "game.h"
#include <math.h>
#include <time.h>
#include <allegro5/allegro_image.h> //알레그로 이미지 애드온
#include <allegro5/allegro_font.h> //알레그로 폰트 애드온
#include <allegro5/allegro_ttf.h> //알레그로 폰트 애드온에서 트루타입 폰트를 지원하기 위한 애드온
#include <allegro5/allegro_primitives.h> //알레그로에서 도형을 지원하기 위한애드온

//전체 디스플레이 크기는 1200x800
//대화창 크기는 340x800
//게임판 + 디스플레이 크기는 860x800

#define WINDOW_WIDTH 860
#define WINDOW_HEIGHT 800
#define BOARD_X 0
#define BOARD_Y 300
#define PAWN_X(j) 11 + j * 99
#define PAWN_Y(i) BOARD_Y + 45 + i * 100

//draw_scene에서 사용하는 p1과 p2의 좌표 (y좌표는 언제나 0)
#define P1_X 160
#define P2_X 460

//draw_scene에서 사용하는 스프라이트 내 동작의 좌표
#define TURN 0
#define WAIT 500
#define DO 1000
#define EVENT 1500

//그래픽 함수에서 사용하는rgb 색상
#define RGB_BLACK al_map_rgb(0,0,0) 
#define RGB_WHITE al_map_rgb(255,255,255)
#define RGB_NAVY al_map_rgb(30,40,70)
#define RGB_KHAKI al_map_rgb(190, 190, 170)
#define RGB_WOOD  al_map_rgb(46,40,48)
#define RGB_RED al_map_rgb(255,0,0)
#define RGB_YELLOW al_map_rgb(255,255,0)
//animate_capturing()에서 사용하는 캡쳐링 딜레이
double capturing_delay;

//디스플레이 비트맵
ALLEGRO_DISPLAY *disp;
ALLEGRO_DISPLAY *sub_disp;
ALLEGRO_BITMAP *preload_board; //보드판 이미지
ALLEGRO_BITMAP *preload_table; //테이블 이미지
ALLEGRO_BITMAP *preload_black; //흑색 말
ALLEGRO_BITMAP *preload_white; //백색 말
ALLEGRO_BITMAP *preload_bp_toon; //백색 말 애니메이션 스프라이트
ALLEGRO_BITMAP *preload_wp_toon; //흑색 말 애니메이션 스프라이트
ALLEGRO_BITMAP *preload_av; //플레이어가 선택 가능한 위치를 나타내는 푸른색 마커
ALLEGRO_BITMAP *preload_pl; //플레이어 커서를 나타내는 붉은 색 마커
ALLEGRO_BITMAP *preload_bg; //draw_scene()에서 사용하는 배경
ALLEGRO_BITMAP *preload_icon; //아이콘

int init_graphics(void); //자주 사용하는 이미지들을 전역변수에 로드
void draw_scene(USER *player, int flag); //상단 화면을 출력하는 함수
void destroy_graphics(void); //알레그로 비트맵을 저장하는 전역변수를 메모리에서 해제하는 함수
void draw_board(BOARD board[]); //하단 보드를 출력하는 함수
int get_selection(int available[], int player_pos, BOARD board[]); //여러 선택지(좌표) 내에서 사용자의 선택을 받는 함수
void animate_capturing(int color, int pos, BOARD board[]); //잡힌 말이 사라지는 애니메이션을 출력하는 함수
void show_turn_info(USER *player, int color); //현재 어떤 플레이어가 턴을 진행하는지를 출력하는 함수
void create_sub_display(void); //다이얼로그 이벤트용 서브 디스플레이를 출력하는 함수