#pragma once
#include <allegro5/allegro_audio.h> //알레그로 오디오 애드온
#include <allegro5/allegro_acodec.h> //알레그로 오디오 코덱 애드온


int bgm_num; //현재 흐르는 bgm의 숫자
double bgm_volume;
double se_volume;
ALLEGRO_SAMPLE *preload_bgm[3];
ALLEGRO_SAMPLE *preload_piece_drop;

int init_audio(void);
void destroy_audios(void);
void play_piecedrop(void);