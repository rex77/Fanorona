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
	preload_icon = al_load_bitmap("Data/Graphic/icon.png"); //����� �κ�

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
	al_destroy_bitmap(preload_icon); //����� �κ�
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
	//�÷��̾ ������ �� �ִ� ��ǥ���� ���� ��, �� ������ �߿��� �÷��̾ �ϳ��� �����ϰ� �Ѵ�.
	//�������� ���� ���ϵ��� ������ ���� ��Ŀ�� �̿��� ǥ���ϸ�, �÷��̾��� Ŀ���� ������ ��Ŀ, �������� Ǫ���� ��Ŀ�� ǥ���Ѵ�.
	//�÷��̾ ������ �� �ִ� ��ġ�� �÷��̾� Ŀ���� ������, ���带 ���ڷ� �޴´�.
	int i = 0;
	bool redraw = false;
	ALLEGRO_BITMAP *cursor_av = preload_av; //�÷��̾ ���� ������ ��ġ�� ǥ���� ��Ŀ (Ǫ����)
	ALLEGRO_BITMAP *cursor_pl = preload_pl; //�÷��̾��� Ŀ�� ��ġ�� ǥ���� ��Ŀ (������) 
	ALLEGRO_FONT *default_font = al_create_builtin_font(); //���� �� ���� ǥ���ϱ� ���� ��Ʈ�� �����Ѵ�. ���� ��Ʈ�� �ҷ����� �ʰ� �˷��׷ο��� �⺻���� �����ϴ� ��Ʈ�� ����Ѵ�.

	//�÷��̾��� �Է½ð��� �����ϴ� Ÿ�̸� game.h�� ���ǵ�
	al_set_timer_count(turn_timer, 0); //Ÿ�̸��� ���� ���� 0���� �ٲ۴�.
	al_start_timer(turn_timer); //Ÿ�̸��� ������ �����Ѵ�.
	
	while (true) {
		al_wait_for_event(queue, &event); //�̺�Ʈ�� ��ٸ���.(�̺�Ʈ�� �� ������ ��ٸ���.)
		switch (event.type) {
		case ALLEGRO_EVENT_TIMER: //Ÿ�̸Ӱ� �������ٸ� (���⼭ Ÿ�̸� �̺�Ʈ�� 2���� �ִ�..)
			//���ѽð��� ��� �ҽ�
			if (event.timer.source == turn_timer && al_get_timer_count(turn_timer) >= 10) {
				return available[0]; //�÷��̾ ù��° �������� ������ ��ó�� ��ȯ�Ѵ�.
			}
			redraw = true; //switch���� ������������ �׷��� ó�� ��ƾ�� �۵���Ų��.
			break;
		case ALLEGRO_EVENT_KEY_DOWN: //�÷��̾��� Ŀ��(������)�� ����Ű �̵��� ���� ���� �󿡼� �̵���Ų��.
			if (event.keyboard.keycode == ALLEGRO_KEY_UP) {
				if (player_pos - 9 >= 0) player_pos-=9; //�÷��̾� Ŀ�� ��ġ�� �������� ��ǥ �� �������� �̵� �����ϴ�.
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
			if (event.keyboard.keycode == ALLEGRO_KEY_ENTER || event.keyboard.keycode == ALLEGRO_KEY_SPACE) { //���Ϳ� �����̽��� �Է��ϸ� ���� ��ƾ�� �����Ѵ�.
				for (i = 0; available[i] != EOA; i++) { //��ȿ�� �˻�
					if (available[i] == player_pos) {   //Ŀ���� �÷��̾ ������ �� �ִ� ��ġ�� �ִ���
						al_destroy_font(default_font); //���ѽð��� ǥ���ϱ� ���� ����ߴ� ��Ʈ ��ü�� �ı��ϰ�
						al_stop_timer(turn_timer); //���� �ð� Ÿ�̸Ӹ� �����Ѵ�. (�� Ÿ�̸Ӹ� �������� ������ ���� ���� �Լ������� �� Ÿ�̸��� �̺�Ʈ�� �ν��ϱ� ����)
						return player_pos; //�÷��̾��� ���ð��� ��ȯ�ϰ� �Լ��� �����Ѵ�.
					}
				}
			}
			break;
		}
		if (redraw) { //�׷��� ó�� ��ƾ (redraw�� ���� �Ǵ� ������ 1/30�ʰ� ������ ��)
			draw_board(board); //��Ŀ�� ������ ���� �׷����� �ϹǷ� ���带 ���� �׸���.
			for (i = 0; available[i] != EOA; i++) { //�� �������� ���ؼ� �Ʒ��� ó��(���� ���� ��Ŀ�� �׸�)�� �����Ѵ�.
				al_draw_bitmap(cursor_av, PAWN_X(available[i] % 9), PAWN_Y(available[i] / 9), 0); //�������� �� ��ġ�� ���� cursor_av(Ǫ���� ��Ŀ)�� �׸���.
			}
			al_draw_bitmap(cursor_pl, PAWN_X(player_pos % 9), PAWN_Y(player_pos / 9), 0); //�÷��̾��� Ŀ�� cursor_pl(������)�� ȭ�� �� �׸���. PAWN_X�� PAWN_Y�� ������ ������ ���� ��ġ�� ����ϴ� ��ũ�� �Լ��̴�.
			al_draw_textf(default_font, RGB_WHITE, 10, BOARD_Y + 10, 0, "%2d sec left..", 10 - al_get_timer_count(turn_timer));//���� Ÿ�̸��� �� ���� ȭ�� ��ܿ� �׸���.
			al_flip_display(); //������ ������ ȭ�鿡 �����Ѵ�.
			redraw = false; //���� �ݺ��� Ÿ�̸� �̺�Ʈ(1/30��)�� �߻����� �ʴ� �̻� ȭ���� �ٽ� �׸��� �ʵ��� ���� ���θ� false�� ��ȯ�Ѵ�.
		}
	}
}

void draw_scene(USER *player, int flag) {
	ALLEGRO_BITMAP *background = preload_bg; //���� �������� �����ؿ´�.
	ALLEGRO_BITMAP *table = preload_table;
	if (player == NULL) {//���� ����� �÷��̾� ������ �ѱ��� �ʾ��� ���� ��游 ����ϰ� ������. �÷��״� �������. (���� ó�� ���� �ÿ� ���)
		al_draw_bitmap(background, 0, 0, 0); //����� �׸���
		return; //�����Ѵ�.
	}
	ALLEGRO_BITMAP *p1_toon = p1->toon; //���� p1�� ��������Ʈ�� �����ؿ´�.
	ALLEGRO_BITMAP *p2_toon = p2->toon; //���� p2�� ��������Ʈ�� �����ؿ´�.
	int i = 0;
	must_init(p1_toon, "p1 sprite");
	must_init(p2_toon, "p2 sprite");

	al_draw_bitmap(background, 0, 0, 0);
	
	if (player == p1) {
		al_draw_bitmap_region(p2_toon, WAIT + 250, 0, 250, 300, P2_X, 0, 0);
		al_draw_bitmap(table, 0, 0, 0); //�÷��̾� 2 �տ� Ź�ڸ� �׸���.
		switch (flag) { //�÷��׿� ���� �÷��̾� 1�� �׸���.
		case 1: //���� �����ϴ� ���
			al_draw_bitmap_region(p1_toon, TURN, 0, 250, 300, P1_X, 0, 0);
			break;
		case 2: //���� �δ� ���
			al_draw_bitmap_region(p1_toon, DO, 0, 250, 300, P1_X, 0, 0);
			break;
		case 3: //�̺�Ʈ �ƾ�(ķ���� ���)
			al_draw_bitmap_region(p1_toon, EVENT, 0, 250, 300, P1_X, 0, 0);
			break;
		default:
			break;
		}
	}
	else if (player == p2) { //���� �ι�° �÷��̾ ������� �Ѵٸ�
		//�÷��̾� 2�� �÷��̾� 1�� Ź�ڿ� ���� ���� ���̾ �����Ƿ� ���� �׸���.
		//�÷��̾� 2�� ����� �÷��̾� 1�� ������� ��Ʈ�� �󿡼� 250�ȼ� �ڿ� �ִ�. �׷��Ƿ� �� ��ġ�� ���Ѵ�.
		switch (flag) { //�÷��׿� ���� �ٸ� ����� �׸���.
		case 1: //���� �����ϴ� ���
			al_draw_bitmap_region(p2_toon, TURN + 250, 0, 250, 300, P2_X, 0, 0);
			break;
		case 2://���� �δ� ���
			al_draw_bitmap_region(p2_toon, DO + 250, 0, 250, 300, P2_X, 0, 0);
			break;
		case 3: //�̺�Ʈ �ƾ�(ķ���� ���)
			al_draw_bitmap_region(p2_toon, EVENT, 0, 250, 300, P2_X, 0, 0);
			break;
		default:
			break;
		}
		al_draw_bitmap(table, 0, 0, 0); //���̺��� �׸���.
		al_draw_bitmap_region(p1_toon, WAIT, 0, 250, 300, P1_X, 0, 0); //�÷��̾� 1�� ����ϴ� ������� �׸���.
	}
	al_flip_display(); //���÷��� ������ �������� ȭ���� ����Ѵ�.

	if (flag == 2 || flag == 3) { //Ư���� ��쿡 (���� �δ� ����� ǥ���ϰų� �̺�Ʈ �ƾ��� ����� ��)
		//ȭ���� ����� �� 0.5�ʸ� ��ٸ���.
		al_rest(0.5); //Sleep(50)�� ����.
	}
}

void animate_capturing(int color, int pos , BOARD board[]) {
	int i = 0, j=0;
	bool redraw = false;
	ALLEGRO_BITMAP *pawn = (color == White) ? preload_wp_toon : preload_bp_toon; //������ ���� ���� ���� ��������Ʈ�� �ٸ��� �����´�
	play_piecedrop();
	for (i = 0; i < 7; i++) {
			draw_board(board); //���带 �׸���.
			al_draw_bitmap_region(pawn, i * 50, 0, 50, 50, PAWN_X(pos % 9), PAWN_Y(pos / 9), 0); //��������Ʈ�� Ư�� ������ �׸���.
			al_flip_display(); //ȭ���� �����Ѵ�.
			al_rest(capturing_delay); //���� �ִϸ��̼��� �����ϱ� ���� ��� �����̸� ������. al_rest()�� Sleep()�� ���� ������ �Ѵ�.
	}
;}

void show_turn_info(USER *player, int color) {
	ALLEGRO_FONT *default_font = al_create_builtin_font();
	al_draw_filled_rectangle(5,5,150,25,al_map_rgba_f(0.2,0.2,0.2,0.3)); //�۾��� �ߺ��̵��� ��濡 ������ �簢��
	al_draw_textf(default_font, color==White ? RGB_WHITE : RGB_BLACK, 10, 10, 0, "%s's turn",player->name); //~�� ���Դϴ� ���
	al_flip_display(); //ȭ���� ���÷��� ���۷� ����
	al_destroy_font(default_font); //��Ʈ ����
}

void create_sub_display(void) {
	//350 x 800 ũ���� ���̾�α׿� ������÷��̸� �����Ѵ�.
	int disp_x, disp_y; //������ ���� ���� ���÷��� ��ġ�� �����ϱ� ���� ��ǥ ����.

	sub_disp = al_create_display(350, 800); //350x800 ũ���� ���÷��̸� �����Ѵ�.
	al_get_window_position(disp, &disp_x, &disp_y); //���� ���÷��� ��ǥ�� �����´�. (al_get_window_position())
	al_set_window_position(sub_disp, disp_x - 350, disp_y); //������ ���� ���÷����� ��ǥ�� ���� ���÷����� ��ǥ���� ��������� ����� ���÷��̸� ������ ��ġ�� �ű��. (al_set_window_position())
	must_init(sub_disp, "subdisplay"); //���� ���÷����� ��ȿ���� �˻��Ѵ�.
	al_set_target_backbuffer(disp); //��Ʈ���� �׸��� ���۸� ���� ���÷��̷� �ٲ۴�.
}