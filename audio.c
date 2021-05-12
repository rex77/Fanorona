#include "audio.h"
#include "main.h"
int init_audio(void) {
	preload_bgm[0] = al_load_sample("Data/Audio/Parting_glass.ogg"); //bgm 배열에 노래를 불러온다. (ALLEGRO_SAMPLE)
	preload_bgm[1] = al_load_sample("Data/Audio/Madagascar.ogg");
	preload_bgm[2] = al_load_sample("Data/Audio/Lemon_tree.ogg");
	preload_piece_drop = al_load_sample("Data/Audio/chess_piece.wav"); //수를 둘 때 재생하는 se
	se_volume = 0.5; //기본 se 볼륨값
	bgm_volume = 0.5; //기본 bgm 볼륨값
	if(!preload_bgm[0] || !preload_bgm[1] || !preload_bgm[2] || !preload_piece_drop)
		return 0;
	return 1;
}
void play_piecedrop(void) {
	ALLEGRO_SAMPLE *se = preload_piece_drop;
	al_play_sample(se, se_volume, 0, 0.5, ALLEGRO_PLAYMODE_ONCE, NULL);
}
void destroy_audios(void) {
	al_destroy_sample(preload_bgm[0]);
	al_destroy_sample(preload_bgm[1]);
	al_destroy_sample(preload_bgm[2]);
	al_destroy_sample(preload_piece_drop);
}