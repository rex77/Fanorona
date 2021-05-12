#include "main.h"
#include "game.h"
#include "graphics.h"
#include "audio.h"
int init_graphics(void) {	
	preload_board = al_load_bitmap("Data/Graphic/Board.png");
	preload_black = al_load_bitmap("Data/Graphic/Blackpawn.png");
	preload_white = al_load_bitmap("Data/Graphic/Whitepawn.png");
	preload_av = al_load_bitmap("Data/Graphic/Cursor_available.png");
	preload_pl = al_load_bitmap("Data/Graphic/Cursor_player.png");
	preload_bg = al_load_bitmap("Data/Graphic/Background_1.png");
	preload_table = al_load_bitmap("Data/Graphic/Table_1.png");
	preload_bp_toon = al_load_bitmap("Data/Graphic/Black_pawn_animation.png");
	preload_wp_toon = al_load_bitmap("Data/Graphic/White_pawn_animation.png");
	preload_icon = al_load_bitmap("Data/Graphic/icon.png"); //변경된 부분

	capturing_delay = 0.04;
	if (!preload_board || !preload_black || !preload_white || !preload_av || !preload_pl)
		return 0;
	return 1;
}

void destroy_graphics(void) {
	al_destroy_bitmap(preload_board);
	al_destroy_bitmap(preload_table);
	al_destroy_bitmap(preload_black);
	al_destroy_bitmap(preload_white);
	al_destroy_bitmap(preload_av);
	al_destroy_bitmap(preload_pl);
	al_destroy_bitmap(preload_bg);
	al_destroy_bitmap(preload_bp_toon);
	al_destroy_bitmap(preload_wp_toon);
	al_destroy_bitmap(preload_icon); //변경된 부분
}

void draw_board(BOARD board[]) { //draw, but won't flip
	int i = 0;

	ALLEGRO_BITMAP *board_image = preload_board;
	ALLEGRO_BITMAP *pawn_black = preload_black;
	ALLEGRO_BITMAP *pawn_white = preload_white;

	al_draw_bitmap(board_image, 0, 300, 0);

	for (i = 0; i < 45; i++) {
			if (board[i] == Black) al_draw_bitmap(pawn_black, PAWN_X(i%9), PAWN_Y(i/9), 0);
			else if (board[i] == White) al_draw_bitmap(pawn_white, PAWN_X(i % 9), PAWN_Y(i / 9), 0);
	}
}

int get_selection(int available[], int player_pos, BOARD board[]) {
	//플레이어가 선택할 수 있는 좌표들을 받은 뒤, 그 선택지 중에서 플레이어가 하나를 선택하게 한다.
	//선택지는 보기 편하도록 보드판 위에 마커를 이용해 표시하며, 플레이어의 커서는 붉은색 마커, 선택지는 푸른색 마커로 표시한다.
	//플레이어가 선택할 수 있는 위치와 플레이어 커서의 시작점, 보드를 인자로 받는다.
	int i = 0;
	bool redraw = false;
	ALLEGRO_BITMAP *cursor_av = preload_av; //플레이어가 선택 가능한 위치에 표시할 마커 (푸른색)
	ALLEGRO_BITMAP *cursor_pl = preload_pl; //플레이어의 커서 위치에 표시할 마커 (붉은색) 
	ALLEGRO_FONT *default_font = al_create_builtin_font(); //남은 초 수를 표시하기 위한 폰트를 생성한다. 따로 폰트를 불러오지 않고 알레그로에서 기본으로 제공하는 폰트를 사용한다.

	//플레이어의 입력시간을 제한하는 타이머 game.h에 정의됨
	al_set_timer_count(turn_timer, 0); //타이머의 현재 값을 0으로 바꾼다.
	al_start_timer(turn_timer); //타이머의 동작을 시작한다.
	
	while (true) {
		al_wait_for_event(queue, &event); //이벤트를 기다린다.(이벤트가 올 때까지 기다린다.)
		switch (event.type) {
		case ALLEGRO_EVENT_TIMER: //타이머가 움직였다면 (여기서 타이머 이벤트는 2개나 있다..)
			//제한시간을 재는 소스
			if (event.timer.source == turn_timer && al_get_timer_count(turn_timer) >= 10) {
				return available[0]; //플레이어가 첫번째 선택지를 선택한 것처럼 반환한다.
			}
			redraw = true; //switch문을 빠져나갔을때 그래픽 처리 루틴을 작동시킨다.
			break;
		case ALLEGRO_EVENT_KEY_DOWN: //플레이어의 커서(붉은색)을 방향키 이동에 따라 보드 상에서 이동시킨다.
			if (event.keyboard.keycode == ALLEGRO_KEY_UP) {
				if (player_pos - 9 >= 0) player_pos-=9; //플레이어 커서 위치는 보드판의 좌표 값 내에서만 이동 가능하다.
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) {
				if (player_pos + 9 < 45) player_pos += 9;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
				if (player_pos - 1 >= 0) player_pos--;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
				if (player_pos + 1 < 45) player_pos++;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_ENTER || event.keyboard.keycode == ALLEGRO_KEY_SPACE) { //엔터와 스페이스를 입력하면 종료 루틴에 돌입한다.
				for (i = 0; available[i] != EOA; i++) { //유효성 검사
					if (available[i] == player_pos) {   //커서가 플레이어가 선택할 수 있는 위치에 있는지
						al_destroy_font(default_font); //제한시간을 표시하기 위해 사용했던 폰트 개체를 파괴하고
						al_stop_timer(turn_timer); //제한 시간 타이머를 중지한다. (이 타이머를 중지하지 않으면 관련 없는 함수에서도 이 타이머의 이벤트를 인식하기 때문)
						return player_pos; //플레이어의 선택값을 반환하고 함수를 종료한다.
					}
				}
			}
			break;
		}
		if (redraw) { //그래픽 처리 루틴 (redraw가 참이 되는 조건은 1/30초가 지났을 때)
			draw_board(board); //마커는 보드의 위에 그려져야 하므로 보드를 먼저 그린다.
			for (i = 0; available[i] != EOA; i++) { //각 선택지에 대해서 아래의 처리(보드 위에 마커를 그림)를 시행한다.
				al_draw_bitmap(cursor_av, PAWN_X(available[i] % 9), PAWN_Y(available[i] / 9), 0); //선택지의 각 위치에 가서 cursor_av(푸른색 마커)을 그린다.
			}
			al_draw_bitmap(cursor_pl, PAWN_X(player_pos % 9), PAWN_Y(player_pos / 9), 0); //플레이어의 커서 cursor_pl(붉은색)를 화면 상에 그린다. PAWN_X와 PAWN_Y는 보드판 위에서 말의 위치를 계산하는 매크로 함수이다.
			al_draw_textf(default_font, RGB_WHITE, 10, BOARD_Y + 10, 0, "%2d sec left..", 10 - al_get_timer_count(turn_timer));//남은 타이머의 초 수를 화면 상단에 그린다.
			al_flip_display(); //버퍼의 내용을 화면에 갱신한다.
			redraw = false; //다음 반복시 타이머 이벤트(1/30초)가 발생하지 않는 이상 화면을 다시 그리지 않도록 갱신 여부를 false로 변환한다.
		}
	}
}

void draw_scene(USER *player, int flag) {
	ALLEGRO_BITMAP *background = preload_bg; //전역 변수에서 복사해온다.
	ALLEGRO_BITMAP *table = preload_table;
	if (player == NULL) {//만약 출력할 플레이어 정보를 넘기지 않았을 때는 배경만 출력하고 끝낸다. 플래그는 상관없다. (게임 처음 시작 시에 사용)
		al_draw_bitmap(background, 0, 0, 0); //배경을 그리고
		return; //종료한다.
	}
	ALLEGRO_BITMAP *p1_toon = p1->toon; //현재 p1의 스프라이트를 복사해온다.
	ALLEGRO_BITMAP *p2_toon = p2->toon; //현재 p2의 스프라이트를 복사해온다.
	int i = 0;
	must_init(p1_toon, "p1 sprite");
	must_init(p2_toon, "p2 sprite");

	al_draw_bitmap(background, 0, 0, 0);
	
	if (player == p1) {
		al_draw_bitmap_region(p2_toon, WAIT + 250, 0, 250, 300, P2_X, 0, 0);
		al_draw_bitmap(table, 0, 0, 0); //플레이어 2 앞에 탁자를 그린다.
		switch (flag) { //플래그에 따라서 플레이어 1을 그린다.
		case 1: //턴을 진행하는 모습
			al_draw_bitmap_region(p1_toon, TURN, 0, 250, 300, P1_X, 0, 0);
			break;
		case 2: //수를 두는 모습
			al_draw_bitmap_region(p1_toon, DO, 0, 250, 300, P1_X, 0, 0);
			break;
		case 3: //이벤트 컷씬(캠페인 모드)
			al_draw_bitmap_region(p1_toon, EVENT, 0, 250, 300, P1_X, 0, 0);
			break;
		default:
			break;
		}
	}
	else if (player == p2) { //만약 두번째 플레이어를 대상으로 한다면
		//플레이어 2가 플레이어 1과 탁자에 비해 뒤쪽 레이어에 있으므로 먼저 그린다.
		//플레이어 2의 모습은 플레이어 1의 모습보다 비트맵 상에서 250픽셀 뒤에 있다. 그러므로 원 위치에 더한다.
		switch (flag) { //플래그에 따라서 다른 모습을 그린다.
		case 1: //턴을 진행하는 모습
			al_draw_bitmap_region(p2_toon, TURN + 250, 0, 250, 300, P2_X, 0, 0);
			break;
		case 2://수를 두는 모습
			al_draw_bitmap_region(p2_toon, DO + 250, 0, 250, 300, P2_X, 0, 0);
			break;
		case 3: //이벤트 컷씬(캠페인 모드)
			al_draw_bitmap_region(p2_toon, EVENT, 0, 250, 300, P2_X, 0, 0);
			break;
		default:
			break;
		}
		al_draw_bitmap(table, 0, 0, 0); //테이블을 그린다.
		al_draw_bitmap_region(p1_toon, WAIT, 0, 250, 300, P1_X, 0, 0); //플레이어 1을 대기하는 모습으로 그린다.
	}
	al_flip_display(); //디스플레이 버퍼의 내용으로 화면을 출력한다.

	if (flag == 2 || flag == 3) { //특정한 경우에 (수를 두는 모습을 표시하거나 이벤트 컷씬을 출력할 때)
		//화면을 출력한 후 0.5초를 기다린다.
		al_rest(0.5); //Sleep(50)와 같음.
	}
}

void animate_capturing(int color, int pos , BOARD board[]) {
	int i = 0, j=0;
	bool redraw = false;
	ALLEGRO_BITMAP *pawn = (color == White) ? preload_wp_toon : preload_bp_toon; //삭제될 말의 색상에 따라 스프라이트를 다르게 가져온다
	play_piecedrop();
	for (i = 0; i < 7; i++) {
			draw_board(board); //보드를 그린다.
			al_draw_bitmap_region(pawn, i * 50, 0, 50, 50, PAWN_X(pos % 9), PAWN_Y(pos / 9), 0); //스프라이트의 특정 영역을 그린다.
			al_flip_display(); //화면을 갱신한다.
			al_rest(capturing_delay); //다음 애니메이션을 진행하기 전에 잠시 딜레이를 가진다. al_rest()는 Sleep()과 같은 역할을 한다.
	}
;}

void show_turn_info(USER *player, int color) {
	ALLEGRO_FONT *default_font = al_create_builtin_font();
	al_draw_filled_rectangle(5,5,150,25,al_map_rgba_f(0.2,0.2,0.2,0.3)); //글씨가 잘보이도록 배경에 투명한 사각형
	al_draw_textf(default_font, color==White ? RGB_WHITE : RGB_BLACK, 10, 10, 0, "%s's turn",player->name); //~의 턴입니다 출력
	al_flip_display(); //화면을 디스플레이 버퍼로 갱신
	al_destroy_font(default_font); //폰트 삭제
}

void create_sub_display(void) {
	//350 x 800 크기의 다이얼로그용 서브디스플레이를 생성한다.
	int disp_x, disp_y; //윈도우 상의 메인 디스플레이 위치를 저장하기 위한 좌표 변수.

	sub_disp = al_create_display(350, 800); //350x800 크기의 디스플레이를 생성한다.
	al_get_window_position(disp, &disp_x, &disp_y); //메인 디스플레이 좌표를 가져온다. (al_get_window_position())
	al_set_window_position(sub_disp, disp_x - 350, disp_y); //생성된 서브 디스플레이의 좌표를 메인 디스플레이의 좌표에서 상대적으로 계산해 디스플레이를 적절한 위치로 옮긴다. (al_set_window_position())
	must_init(sub_disp, "subdisplay"); //서브 디스플레이의 유효성을 검사한다.
	al_set_target_backbuffer(disp); //비트맵을 그리는 버퍼를 메인 디스플레이로 바꾼다.
}