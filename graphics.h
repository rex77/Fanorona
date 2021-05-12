#pragma once
#include "main.h"
#include "game.h"
#include <math.h>
#include <time.h>
#include <allegro5/allegro_image.h> //�˷��׷� �̹��� �ֵ��
#include <allegro5/allegro_font.h> //�˷��׷� ��Ʈ �ֵ��
#include <allegro5/allegro_ttf.h> //�˷��׷� ��Ʈ �ֵ�¿��� Ʈ��Ÿ�� ��Ʈ�� �����ϱ� ���� �ֵ��
#include <allegro5/allegro_primitives.h> //�˷��׷ο��� ������ �����ϱ� ���Ѿֵ��

//��ü ���÷��� ũ��� 1200x800
//��ȭâ ũ��� 340x800
//������ + ���÷��� ũ��� 860x800

#define WINDOW_WIDTH 860
#define WINDOW_HEIGHT 800
#define BOARD_X 0
#define BOARD_Y 300
#define PAWN_X(j) 11 + j * 99
#define PAWN_Y(i) BOARD_Y + 45 + i * 100

//draw_scene���� ����ϴ� p1�� p2�� ��ǥ (y��ǥ�� ������ 0)
#define P1_X 160
#define P2_X 460

//draw_scene���� ����ϴ� ��������Ʈ �� ������ ��ǥ
#define TURN 0
#define WAIT 500
#define DO 1000
#define EVENT 1500

//�׷��� �Լ����� ����ϴ�rgb ����
#define RGB_BLACK al_map_rgb(0,0,0) 
#define RGB_WHITE al_map_rgb(255,255,255)
#define RGB_NAVY al_map_rgb(30,40,70)
#define RGB_KHAKI al_map_rgb(190, 190, 170)
#define RGB_WOOD  al_map_rgb(46,40,48)
#define RGB_RED al_map_rgb(255,0,0)
#define RGB_YELLOW al_map_rgb(255,255,0)
//animate_capturing()���� ����ϴ� ĸ�ĸ� ������
double capturing_delay;

//���÷��� ��Ʈ��
ALLEGRO_DISPLAY *disp;
ALLEGRO_DISPLAY *sub_disp;
ALLEGRO_BITMAP *preload_board; //������ �̹���
ALLEGRO_BITMAP *preload_table; //���̺� �̹���
ALLEGRO_BITMAP *preload_black; //��� ��
ALLEGRO_BITMAP *preload_white; //��� ��
ALLEGRO_BITMAP *preload_bp_toon; //��� �� �ִϸ��̼� ��������Ʈ
ALLEGRO_BITMAP *preload_wp_toon; //��� �� �ִϸ��̼� ��������Ʈ
ALLEGRO_BITMAP *preload_av; //�÷��̾ ���� ������ ��ġ�� ��Ÿ���� Ǫ���� ��Ŀ
ALLEGRO_BITMAP *preload_pl; //�÷��̾� Ŀ���� ��Ÿ���� ���� �� ��Ŀ
ALLEGRO_BITMAP *preload_bg; //draw_scene()���� ����ϴ� ���
ALLEGRO_BITMAP *preload_icon; //������

int init_graphics(void); //���� ����ϴ� �̹������� ���������� �ε�
void draw_scene(USER *player, int flag); //��� ȭ���� ����ϴ� �Լ�
void destroy_graphics(void); //�˷��׷� ��Ʈ���� �����ϴ� ���������� �޸𸮿��� �����ϴ� �Լ�
void draw_board(BOARD board[]); //�ϴ� ���带 ����ϴ� �Լ�
int get_selection(int available[], int player_pos, BOARD board[]); //���� ������(��ǥ) ������ ������� ������ �޴� �Լ�
void animate_capturing(int color, int pos, BOARD board[]); //���� ���� ������� �ִϸ��̼��� ����ϴ� �Լ�
void show_turn_info(USER *player, int color); //���� � �÷��̾ ���� �����ϴ����� ����ϴ� �Լ�
void create_sub_display(void); //���̾�α� �̺�Ʈ�� ���� ���÷��̸� ����ϴ� �Լ�