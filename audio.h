#pragma once
#include <allegro5/allegro_audio.h> //�˷��׷� ����� �ֵ��
#include <allegro5/allegro_acodec.h> //�˷��׷� ����� �ڵ� �ֵ��


int bgm_num; //���� �帣�� bgm�� ����
double bgm_volume;
double se_volume;
ALLEGRO_SAMPLE *preload_bgm[3];
ALLEGRO_SAMPLE *preload_piece_drop;

int init_audio(void);
void destroy_audios(void);
void play_piecedrop(void);