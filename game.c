//���� ������ ���õ� �Լ� &	����
#include "graphics.h"
#include "main.h"
#include "game.h"
#include "user.h"
#include "audio.h"
#pragma warning(disable:4996)

void init_board(BOARD board[])
{
	int i = 0;
	//��� 0~17, 18, 20, 23, 25 (�浹) �ϴ� 19, 21, 24, 26, 27~44 (�鵹)
	for (i = 0; i < 18; i++) {
		board[i] = Black; //�浹
	}
	board[18] = board[20] = board[23] = board[25] = Black; //�߰� ��
	board[22] = Empty;
	board[19] = board[21] = board[24] = board[26] = White;
	for (i = 27; i < 45; i++) {
		board[i] = White; //�鵹
	}
}

void play_game(int mode) {
	int p1_color = 0, lv = 0, i = 0, now_turn = White, congest = 0, captured_pawn = 0;
	DIALOG_STATE dialog_state = In_progress;
	STATE state = No_win;
	BOARD board[45] = { 0 };

	//���� ���� �� ��忡 ������� ���������� �ʱ�ȭ�ϴ� �κ�
	init_board(board); //���� �ʱ�ȭ
	p1 = &local_user; //���� ���ӿ��� ù��° �÷��̾��� ������ �����Ѵ�.

	turn_timer = al_create_timer(1); //�� Ÿ�̸Ӹ� �����Ѵ�.
	al_register_event_source(queue, al_get_timer_event_source(turn_timer)); //�� Ÿ�̸Ӹ� �̺�Ʈ ť�� ����Ͽ� ���� �̺�Ʈ�� ���� �� �ֵ��� �����.

	draw_board(board); //���÷��� ���ۿ� ���� ���带 �׸���.
	draw_scene(NULL, 0); //���÷��� ���ۿ� ���� ��� ȭ��(���)�� �׸���.

	//ķ���� ��峪 �����÷��� ����� ��� AI�� ���̵��� �����Ѵ�. && �����Լ� ��� �غ� �Ѵ�.
	if (mode == Campaign || mode == Freeplay) {
		srand((unsigned int)time(NULL)); //�����Լ� ����� ���� �õ尪 �غ�
		lv = set_cpu_level(); //cpu ������ �����Ѵ�.
	}

	//���� ������ ��/�İ��� �����ϰ�, ����� p1_color�� �����Ѵ�.
	p1_color = whos_first();

	/*****************************������*******************************/
	switch (mode) {
	case Campaign:
		create_sub_display(); //���̾�α� ������ ���� ���� ���÷��̸� �����Ѵ�.

		p2 = &cpu;
		while (state == No_win) { //�ºΰ� ���� ���� ���� �Ʒ��� �ڵ带 �ݺ��Ѵ�.
			if (now_turn == p1_color)
				captured_pawn = player_move(now_turn, p1, board); //�÷��̾��� ���� �׳� �����Ѵ�.
			else {
				captured_pawn = computer_move(now_turn, p2, lv, board); //��ǻ���� ���� �����ϴ� �Լ��̴�.

				//���̾�α� �̴ϰ���
				if ((rand() % 10) < 4 && dialog_state == In_progress) { //��ǻ���� ���� ������ 40% Ȯ���� ���̾�α� �̴ϰ����� ���ڱ� ����ȴ�.
					dialog_state = campaign_chat(lv);

					if (dialog_state == Successed) { //���̾�α� �̴ϰ����� ���������� Ŭ�������� ��� (5ȸ�� ������ ������ 0 �̻��� ���)
						draw_scene(p2, 3); //������ �̺�Ʈ �ƾ��� �����ش�. (��� ȭ��)
						al_show_native_message_box(NULL, "Fanorona", "Dialog end", "The opponent is satisfied with the great conversation. Dialog event successfully ended. Keep going back to play", NULL, ALLEGRO_MESSAGEBOX_WARN); //����Ƽ�� �޽��� �ڽ��� �̿��� ���̾�α� �̴ϰ����� Ŭ���������� �˸���.
						dialog_state = Nomore;
					}
					else if (dialog_state == Failed) { //���̾�α� �̴ϰ����� �������� ��� (5ȸ�� ������ ���� ������ 0 ���Ϸ� �������� ���)
						al_show_native_message_box(NULL, "Fanorona", "Dialog end", "The opponent is pissed off now by your arrogant words. You lose.", NULL, ALLEGRO_MESSAGEBOX_ERROR); //����Ƽ�� �޽��� �ڽ��� �̿��� ���̾�α� ������ ���������� �˸���.
					//���̾�α� �̺�Ʈ ���� �� ���� ������ ����� ��� ���� ������ �����ǿ��� �÷��̾��� ���� �����Ͽ� ���� check_board()���� �÷��̾ ���� �����.
						for (i = 0; i < 45; i++) {
							if (board[i] == p1_color)
								board[i] = Empty;
						}
						break;
					}
				}
			}
			if (captured_pawn == 0)
				congest++; //��ü ���� �� ���� ������Ų��.
			else
				congest = 0;

			if (congest > 15) {//���� ��ü ������ ä�� 15�� �̻� �����ٸ�
				if (al_show_native_message_box(NULL, "Fanorona", "Draw", "Wanna end this game?", NULL, ALLEGRO_MESSAGEBOX_YES_NO) == 1) { //����Ƽ�� ���̾�α׸� �̿��� �÷��̾�(local)���� ���ºθ� �����϶�
				//���� ������ ���º� ���·� ���� ������Ʈ
					for (i = 0; i < 45; i++) //check_board()���� ���ºθ� �Ǵ��ϱ� ���� ���� -- ��� ���� ���忡 ��ġ���� ���� ��
						board[i] = Empty;
				}
				else {
					//���� ������ ��ü �� ���� ó������ ����Ѵ�.
					congest = 0;
				}
			}
			state = check_board(board); //������ ���¸� üũ�ϰ� state ������ �����Ѵ�.
			now_turn *= -1; //���� �ٲ۴�. (���� ������ �ٲ۴�.)
		}
		al_destroy_display(sub_disp); //���̾�α� �̺�Ʈ������ ����� ���÷��̸� �����Ѵ�.
		break;
	case Freeplay:
		p2 = &cpu;
		while (state == No_win) { //�ºΰ� ���� ���� ���� �Ʒ� �ڵ带 �ݺ��Ѵ�.
			if (now_turn == p1_color)
				captured_pawn = player_move(now_turn, p1, board); //�÷��̾��� ���� ����
			else
				captured_pawn = computer_move(now_turn, p2, lv, board); //��ǻ���� ���� ����
			//������ʹ� Two_player�� ����
			if (captured_pawn == 0)
				congest++; //��ü ���� �� ���� ������Ų��.
			else
				congest = 0;
			if (congest > 15) {//���� ��ü ������ ä�� 15�� �̻� �����ٸ�
				if (al_show_native_message_box(NULL, "Fanorona", "Draw", "Wanna end this game?", NULL, ALLEGRO_MESSAGEBOX_YES_NO) == 1) { //����Ƽ�� ���̾�α׸� �̿��� �÷��̾�(local)���� ���ºθ� �����϶�
				//���� ������ ���º� ���·� ���� ������Ʈ
					for (i = 0; i < 45; i++) //check_board()���� ���ºθ� �Ǵ��ϱ� ���� ����
						board[i] = Empty;
				}
				else {
					//���� ������ ��ü �� ���� ó������ ��Ƹ�
					congest = 0;
				}
			}
			state = check_board(board); //������ ���¸� üũ�ϰ� state ������ �����Ѵ�.
			now_turn *= -1; //���� �ٲ۴�. (���� ������ �ٲ۴�.)
		}
		break;
	case Two_player: //�������� 2�� ���
		init_guest(); // ���� ����ü�� ����Ǿ��ִ� USER guest�� �⺻������ �ʱ�ȭ
		p2 = &guest;  // �� ���ӿ��� ���(�ι�°) �÷��̾ ����
		//���� ����
		while (state == No_win) {
			captured_pawn = player_move(now_turn, (p1_color == now_turn) ? p1 : p2, board);
			if (captured_pawn == 0)
				congest++;
			else
				congest = 0;
			if (congest > 15) {
				if (al_show_native_message_box(NULL, "Fanorona", "Draw", "Wanna end this game?", NULL, ALLEGRO_MESSAGEBOX_YES_NO) == 1) { //����Ƽ�� ���̾�α׸� �̿��� �÷��̾�(local)���� ���ºθ� �����϶�
				//���� ������ ���º� ���·� ���� ������Ʈ
					for (i = 0; i < 45; i++) //check_board()���� ���ºθ� �Ǵ��ϱ� ���� ����
						board[i] = Empty;    //�������� ��� ĭ�� ��ĭ�̾�� �Ѵ�.
				}
				else {
					//���� ������ ��ü �� ���� ó������ ����Ѵ�.
					congest = 0;
				}
			}
			state = check_board(board); //������ ���¸� üũ�ϰ� state ������ �����Ѵ�.
			now_turn *= -1; //���� �ٲ۴�. (���� ������ �ٲ۴�.)
		}
		break;
	default:
		printf("entered wrong mode value\n"); //��� �Է� ����
		exit(0);
	}
	//������ ������ ��
	switch (state) { //���� �������� check_board()�� ��ȯ�� ���·� �Ǵ�
	case Black_win://���� ����� �̰�ٸ�
		al_show_native_message_box(NULL, "Fanorona", "Black win", "Black win!", NULL, ALLEGRO_MESSAGEBOX_QUESTION);//����Ƽ�� ���̾�α׷� ����� �̱��� �˸�
		p1_color == White ? p1->lose++ : p1->win++; //p1�� ������ local_user, local_user�� ����(�¸�) ������Ʈ
		break;
	case White_win://����� �̰�ٸ�
		al_show_native_message_box(NULL, "Fanorona", "White win", "White win!", NULL, ALLEGRO_MESSAGEBOX_QUESTION);//����Ƽ�� ���̾�α׷� ����� �̱��� �˸�
		p1_color == White ? p1->win++ : p1->lose++; //local_user�� ����(�й�) ������Ʈ
		break;
	case Draw://���ºζ��
		al_show_native_message_box(NULL, "Fanorona", "Draw", "It's a tie!", NULL, ALLEGRO_MESSAGEBOX_QUESTION); //����Ƽ�� ���̾�α׷� ���ºΰ� ���� �˸�
		p1->tie++; //local_user�� ����(���º�) ������Ʈ
		break;
	default:
		break;
	}
	if (login)
		update_user_result(); //db�� �α��ε� ������ ������ �����Ѵ�.
}

int set_cpu_level(void) {
	ALLEGRO_FONT *font = al_load_ttf_font("data/font/Menu.ttf", 35, 0);
	int redraw = false, cursor = 0;
	//ȭ�� ���� ��ƾ�� ����Ѵ�.
	while (true) {
		al_wait_for_event(queue, &event);
		switch (event.type) {
		case ALLEGRO_EVENT_TIMER:
			redraw = true;
			break;
		case ALLEGRO_EVENT_KEY_DOWN:
			if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) { //���� ȭ��ǥ�� ���ȴٸ�
				if (cursor > 0) //Ŀ�� ��ȿ�� �˻� �� �������� �̵�
					cursor--; 
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) { //������ ȭ��ǥ�� ���ȴٸ�
				if (cursor < 2) //Ŀ�� ��ȿ�� �˻� �� �������� �̵�
					cursor++;
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) { //���� Ű�� �ԷµǾ��ٸ�
				init_cpu(cursor); //������ ���� ���� cpu ����ü�� �ʱ�ȭ
				al_destroy_font(font); //��Ʈ�� �޸𸮿��� �����ϰ�
				return cursor; //�Լ��� ������.
			}
			break;
		}
		if (redraw) { //ȭ�� ���� ��ƾ
			al_draw_filled_rectangle(220, BOARD_Y + 150, 640, BOARD_Y + 350, RGB_KHAKI); //��� �簢�� �׸���
			al_draw_multiline_text(font, RGB_BLACK, 285, 490, 360, 30, 0, "Which ENEMY do you wanna beat?");
			al_draw_text(font, (cursor == 0) ? RGB_NAVY : RGB_WHITE, 310, 590, ALLEGRO_ALIGN_CENTER, "LV0"); //ALLEGRO_ALIGN_CENTER �÷��� : �ؽ�Ʈ �߾�����
			al_draw_text(font, (cursor == 1) ? RGB_NAVY : RGB_WHITE, 430, 590, ALLEGRO_ALIGN_CENTER, "LV1");
			al_draw_text(font, (cursor == 2) ? RGB_NAVY : RGB_WHITE, 550, 590, ALLEGRO_ALIGN_CENTER, "LV2");
			al_flip_display();
			redraw = false;
		}
	}
}

int whos_first(void) {
	ALLEGRO_FONT *font = al_load_ttf_font("data/font/Menu.ttf", 35, 0);
	int redraw = false, cursor = White;
	//ȭ���� ��� �����ؾ� �ϹǷ� �ݺ����� �ʿ���
	while (true) {
		al_wait_for_event(queue, &event); //�̺�Ʈ ť(��������)���� ���� �߻��� �̺�Ʈ�� �޴´�.
		switch (event.type) { //���� �̺�Ʈ��
		case ALLEGRO_EVENT_TIMER: //Ÿ�̸ӿ��� �߻��� �̺�Ʈ��� (1/30�ʰ� �����ٸ�)
			redraw = true; //ȭ���� �����϶�(�׸��� ��ƾ�� �Ʒ���)
			break;
		case ALLEGRO_EVENT_KEY_DOWN://Ű���忡�� �߻��� �̺�Ʈ���
			//� Ű�� ���ȴ��� �Ǻ��϶�
			if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) { //���� ȭ��ǥ�� ���ȴٸ�
				cursor = White; //�÷��̾��� ���� ������ ���
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) { //������ ȭ��ǥ�� ���ȴٸ�
				cursor = Black; //�÷��̾��� ���� ������ ������
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) { //���� Ű�� �ԷµǾ��ٸ�
				al_destroy_font(font); //�Լ��� ���� �غ� (�ҷ��� ��Ʈ�� �޸𸮿��� ����)
				return cursor; //�÷��̾��� ������ ��ȯ�ϰ� �Լ��� ����
			}
			break;
		}
		if (redraw) { //���� ȭ�� ������ �ؾ� �Ѵٸ�
			al_draw_filled_rectangle(220, BOARD_Y + 150, 640, BOARD_Y + 350, RGB_KHAKI); //Ȳ������ ä���� �簢���� �־��� ��ǥ���� �°� ���÷��� ���ۿ� �׸���
			al_draw_multiline_text(font, RGB_BLACK, 285, 490, 300, 30, 0, "Who will be your loyal soldiers, your majesty?"); //���÷��� ���ۿ� �ؽ�Ʈ�� �׸���
			al_draw_text(font, (cursor == White) ? RGB_NAVY : RGB_WHITE, 290, 590, 0, "WHITE"); //�÷��̾��� ���ÿ� ���� ������ �ٸ��� "WHITE" ���ڸ� �׸��� (���� �� ����, ������ �ȵǾ����� ���)
			al_draw_text(font, (cursor == Black) ? RGB_NAVY : RGB_WHITE, 467, 590, 0, "BLACK"); //�÷��̾��� ���ÿ� ���� ������ �ٸ��� "BLACK" ���ڸ� �׸��� (���� �� ����, ������ �ȵǾ����� ���)
			al_flip_display();//���÷��̸� �����϶� (���÷��� ���ۿ� �ִ� ������ ���� ���÷��̿� ǥ��)
			redraw = false; //ȭ�� ���� ������ �ٽ� �������� �ʱ�ȭ. ���� Ÿ�̸� �̺�Ʈ �߻� �� (1/30�ʰ� ������ ��) 
		}
	}
}

int player_move(int color, USER *player, BOARD board[]) { //��ȯ���� ������ �� ����
	int i = 0, j = 0, k = 0;
	int available_map[22] = { 0 }, trail[22] = { EOA, 0 };
	int new_pos = 0, cur_pos = 0;
	DIRECTION dir_map[8] = { North, Northwest, West, Southwest, South,  Southeast,  East, Northeast };
	int  captured = 0, move_type = 0;
	bool sequence = false;
	DIRECTION dir = 0;

	//��� ȭ���� �÷��̾ ���� �ΰ� �ִ� ���·� �׸���, ȭ�� ��ܿ� ���� � �÷��̾ �����ϰ� �ִ��� ������ �ۼ��� �� ���÷��� ǥ��
	draw_scene(player, 1);
	show_turn_info(player, color);

	//���� ������ �� ��ü �����ǿ��� ĸ�ĸ� ����� �̵� ������ �� �˻�
	for (i = 0; i < 45; i++) {
		if (board[i] == color) {
			for (j = 0; j < 8; j += (i % 2 + 1)) {//j+=(i%2+1) ? ¦��ĭ�� 8�������� �̵� ����(i++), Ȧ�� ĭ�� 4�������θ� �̵� ���� (i+=2)
				if (is_moveable(i, i + dir_map[j], dir_map[j], board) && //dir_map[j] �������� �̵��� �� �ִ°�?
					(is_catchable(i, i + dir_map[j] * 2, dir_map[j], board) || is_catchable(i, i + dir_map[j] * -1, dir_map[j], board)) //�̵��� ��ġ���� (������ ��) ������ Ȥ�� ������ ���� �� �ִ� ���� �ִ°�?
					) {
					available_map[k++] = i; //�÷��̾ �̵� ������ ���� �迭�� ���� ��ǥ(�� �ִ� ��)�� �߰��϶�
					break;
				}
			}
		}
	}
	available_map[k] = EOA; //��ȿ�� �������� �Է��� �������� �˸����� EOA(End of Array)�� �߰��Ѵ�.

	//ĸ�ĸ� ����� ����ī ���긦 �����ؼ� ����
	if (available_map[0] != EOA) { //���� ĸó�� ���갡 ������ ���� �ϳ� �̻� �ִٸ� (�迭�� ������� �ʾҴٸ�)
		cur_pos = get_selection(available_map, 22, board); //get_selection���� available_map�� �ִ� ��ǥ �� �ϳ��� �÷��̾� �������� �޾� cur_pos�� �����Ѵ�.
		do {
			//������ ���� ���� ĸ�ĸ� ���� ���� (�̵� �Ŀ��� ������ �� �ִ� ���� ��� �����ִ� �� �ݺ��Ѵ�.)

			//������ ���� ���� �̵����� ���� (�� ĭ �˻�, �ߺ� �̵� �˻�, ���� ���� �˻�)
			for (i = 0, j = 0; i < 8; i += cur_pos % 2 + 1) {
				if (is_moveable(cur_pos, cur_pos + dir_map[i], dir_map[i], board) && //dir_map[i]�� �������� �̵��� �����Ѱ�? (�� ĭ�ΰ�?)
					is_first_time(cur_pos + dir_map[i], trail) && //�̵� �� ��ĭ�� Ȥ�� ������ �� ���� �ִ°�
					(is_catchable(cur_pos, cur_pos + dir_map[i] * 2, dir_map[i], board) == true || //�� ĭ���� �̵����� �� ������� ���� �� �ִ� ���� �ִ°�?
						is_catchable(cur_pos, cur_pos + dir_map[i] * -1, dir_map[i], board) == true))  //Ȥ�� ������ ���� �� �ִ� ���� �ִ°�?
					available_map[j++] = cur_pos + dir_map[i]; //������ ���ǿ� �´´ٸ� ���� ��ǥ�� �������� �����϶�
			}
			if (trail[0] != EOA) {
				available_map[j++] = cur_pos;
			}
			available_map[j] = EOA; //��ȿ�� �������� ���� �˸�

			//�÷��̾��� ������ ���� & ���忡�� ������ �̵� & ������ �������� � �������� �̵��ߴ��� Ȯ���ϱ�
			new_pos = get_selection(available_map, cur_pos, board);
			play_piecedrop(); //���� �̵��ϴ� �Ҹ��� ���
			board[cur_pos] = Empty;
			board[new_pos] = color;
			dir = new_pos - cur_pos;

			if (new_pos == cur_pos)
				return captured;

			//�̵� ���Ŀ� ������ ����
			//�̵��� ��ġ���� ������ Ȥ�� �� ������ �������� Ȯ���Ѵ�. (�������� �̵��� �� �ִ� ���� ����� ĭ�� Ȯ���Ѵ�.)
			move_type = 0; //������/�� ���θ� ��Ÿ���� ������ 0���� �ʱ�ȭ
			if (is_catchable(new_pos, new_pos + dir, dir, board)) //������ ��ġ�� ���� ����
				move_type += 1; //move_type = 1
			if (is_catchable(new_pos, new_pos + dir * -2, dir, board)) //�� ��ġ�� ���� ����
				move_type -= 1; //move_type = -1

			if (move_type == 0) { //������/���� �� �� ������ ��(������ move_type�� 0�� �ÿ�) �÷��̾��� ������ �޴´�.
				//������� ���� �Ǵ� ��ġ�� �迭�� �߰��ؼ� �÷��̾��� ������ �޵��� get_selection���� �ѱ�
				available_map[0] = new_pos + dir;
				available_map[1] = new_pos + dir * -2;
				available_map[2] = EOA;
				if (get_selection(available_map, new_pos, board) == new_pos + dir) //get_selection���� ���� ���� ������ ��ġ�� ���ٸ�
					move_type = 1; //�����带 �����ϵ��� ���� ����
				else
					move_type = -1; //���� �����ϵ��� ���� ����
			}
			if (move_type == 1) {//�����带 ������ �� ������
				for (i = 1; board[new_pos + dir * i] == color * -1; i++) {  //�������� �ִ� �̾��� ����� �� ��ü�� �������(dir * i) ������ �����Ѵ�, ���� �߰��� Empty�� �ڽ��� ���� �ִٸ� �ݺ��� ����ȴ�.
					if (is_catchable(new_pos, new_pos + dir * i, dir, board)) //���� ������ �� �ִ� ���̶��
					{
						board[new_pos + dir * i] = Empty; //�� ��ġ�� ��ĭ���� ��ȯ�Ѵ�
						animate_capturing(color*-1, new_pos + dir * i, board); //�� ���� ���� �����Ǵ� �ִϸ��̼��� �����Ѵ�
						captured++;
					}
				}
			}
			else if (move_type == -1) { //���� ������ �� ���� ��
				for (i = 2; board[new_pos + dir * -i] == color * -1; i++) { //�������� �ִ� �̾��� ����� �� ��ü�� �������(dir * i) ������ �����Ѵ�.
					if (is_catchable(new_pos, new_pos + dir * -i, dir, board)) //������� ����
					{
						board[new_pos + dir * -i] = Empty;
						animate_capturing(color*-1, new_pos + dir * -i, board);
						captured++;
					}
				}
			}

			for (i = 0; trail[i] != EOA; i++);//�迭�� ������ i�� ������Ų��.
			trail[i] = cur_pos; //�迭�� ������ ��ҿ� ���� ��ġ�� �߰��Ѵ�.
			trail[i + 1] = EOA; //�ٽ� EOA�� �߰��Ѵ�.

			sequence = false;
			for (i = 0; i < 8; i += (new_pos % 2 + 1)) {
				if (is_moveable(new_pos, new_pos + dir_map[i], dir_map[i], board) && //���� ��ġ(�̵��� ��ġ)���� �̵� �����ϰ� ������ ��ġ�� �ƴϸ�
					is_first_time(new_pos + dir_map[i], trail) && //������ Ȥ�� ������ ������ �� �ִ� ���� �ϳ� �̻� �ִ°�
					(is_catchable(new_pos, new_pos + dir_map[i] * 2, dir_map[i], board) == true || is_catchable(new_pos, new_pos + dir_map[i] * -1, dir_map[i], board) == true))
				{
					sequence = true; //�������� ĸ�ĸ� ���긦 ������ �� �ֵ��� �Ѵ�.
					cur_pos = new_pos; //���� '���� ��ġ'�� �����Ѵ�.
					break; //�ϳ��� ������ �ǹǷ� ������ ĭ�� �˻��� �ʿ� ����, �ݺ����� ��������
				}
			}
		} while (sequence); //sequence�� true�̸� ���� ���� ���ؼ� ĸ�ĸ� ���긦 ��� �����Ѵ�.
	}
	else {	//����� ���� ������ �� �ִ� ���� �ϳ��� ���ٸ�
	//���带 ���� �˻��ؼ� ����ī ���긦 �����Ѵ�. (���� ������ �ʴ� �̵�, ĸó�� ���갡 �̵��� �� ���� ���� ��� �����ϴ�.)
		for (i = 0; i < 45; i++) {
			if (board[i] == color) {
				for (j = 0; j < 8; j += (i % 2 + 1)) {
					if (is_moveable(i, i + dir_map[j], dir_map[j], board)) { //�̵� ������ ��ĭ�� �ִ� ���� �˻�
						available_map[k++] = i; //�̵� ������ ���� �迭�� ���� ��ǥ�� �߰��ض�
						break;
					}
				}
			}
		}
		available_map[k] = EOA; //��ȿ�� ������ �Է��� ����

		cur_pos = get_selection(available_map, 22, board); //������� ������ �޴´�.
		for (i = 0, j = 0; i < 8; i += cur_pos % 2 + 1) {//������ ���� ���ؼ��� �ٽ� �̵� ������ ��ĭ�� �˻�
			if (is_moveable(cur_pos, cur_pos + dir_map[i], dir_map[i], board)) { //������ ���� ����
				available_map[j++] = cur_pos + dir_map[i];
			}
		}
		available_map[j] = EOA;
		new_pos = get_selection(available_map, cur_pos, board); //�̵��� ��ġ�� ����
		play_piecedrop(); //�̵� ���� ���
		board[cur_pos] = Empty; //������ �������� ����
		board[new_pos] = color;
	}
	draw_scene(player, 2); //��� ȭ�鿡 �÷��̾ ���� �δ� ����� �׸���. (0.5�ʵ��� ������)
	return captured;//������� ������ ���� ��ȯ�ϰ� player_move()�� �����Ѵ�.
}

int computer_move(int color, USER *cpu, int level, BOARD board[]) {
	int i = 0, j = 0, k = 0, cnt = 0;
	int available_map[22] = { 0 }, score_map[22] = { 0 }, trail[22] = { EOA, 0 }, move_map[22] = { EOA, 0 };
	int temp_trail[22] = { EOA }, temp_move[22] = { 0 };
	int new_pos = 0, cur_pos = 0, max_score = 0;
	DIRECTION dir_map[8] = { North, Northwest, West, Southwest, South,  Southeast,  East, Northeast };
	int  captured = 0, move_type = 0;
	bool sequence = false;
	DIRECTION dir = 0;

	//��� ȭ���� cpu�� ���� �ΰ� �ִ� ���·� �׸���, ȭ�� ��ܿ� ���� � �÷��̾ �����ϰ� �ִ��� ������ �ۼ��� �� ���÷��� ǥ��
	draw_scene(cpu, 1);
	show_turn_info(cpu, color);

	//player_move�� �⺻ ���� ����� ����.

	//���� ������ �� ��ü �����ǿ��� ĸ�ĸ� ����� �̵� ������ �� �˻�
	for (i = 0; i < 45; i++) {
		if (board[i] == color) {
			for (j = 0; j < 8; j += (i % 2 + 1)) {//j+=(i%2+1) ? ¦��ĭ�� 8�������� �̵� ����(i++), Ȧ�� ĭ�� 4�������θ� �̵� ���� (i+=2)
				if (is_moveable(i, i + dir_map[j], dir_map[j], board) && //dir_map[j] �������� �̵��� �� �ִ°�?
					(is_catchable(i, i + dir_map[j] * 2, dir_map[j], board) || is_catchable(i, i + dir_map[j] * -1, dir_map[j], board)) //�̵��� ��ġ���� (������ ��) ������ Ȥ�� ������ ���� �� �ִ� ���� �ִ°�?
					) {
					available_map[k++] = i; //�÷��̾ �̵� ������ ���� �迭�� ���� ��ǥ(�� �ִ� ��)�� �߰��϶�
					break;
				}
			}
		}
	}
	available_map[k] = EOA; //��ȿ�� �������� �Է��� �������� �˸����� EOA(End of Array)�� �߰��Ѵ�.

	//ĸ�ĸ� ����� ����ī ���긦 �����ؼ� ����
	if (available_map[0] != EOA) { //���� ĸó�� ���갡 ������ ���� �ϳ� �̻� �ִٸ� (�迭�� ������� �ʾҴٸ�)
		
		//�̵���ų ���� �����Ѵ�.
		//�� switch���� player_move�� get_selection()�� ��ü�Ѵ�.
		switch (level) {
		case 0:	case 1: default: //���� 0, 1�� AI������ ������ ���� �������� �����Ѵ�.
			cur_pos = random_select(available_map);
			break;
		case 2: //���� 2�� ai�� calculate_pawn�� �̿��ؼ� ĸó�� ���갡 ������ ��� ���� ����ϰ� �� �� �ְ� ������ ���� �� �ִ� �ϳ��� �����Ѵ�.
			for (i = 0; available_map[i] != EOA; i++) { //�̵� ������ ��ü ���� ���ؼ� ���
#ifdef DEBUG 
				printf("\n\n�˻��� �� : [%d]\n", available_map[i]);
#endif
				score_map[i] = calculate_pawn(available_map[i], color, 0, temp_trail, board, temp_move); //�� ���� �ִ� ������ score_map�� �����Ѵ�.
				temp_trail[0] = EOA; //temp_trail�� calculate_pawn ���� �̵� ���� ���� �迭�̴�.
			}
			score_map[i] = EOA;
			for (i = 0; score_map[i] != EOA; i++) { 
				if (score_map[i] > max_score) { //�ִ� ������ ã�´�.
					max_score = score_map[i]; //�ִ� ������ �����Ѵ�.
					cur_pos = available_map[i]; //���� ��ġ�� �� ���� �����Ѵ�.
				}
			}

#ifdef DEBUG 
			printf("\n--------------movement ���ſ� �˻�------------------\n");
#endif
			calculate_pawn(cur_pos, color, 0, temp_trail, board, move_map); //������ ���� ���� �ٽ� �ѹ� calculate_pawn()�� �����Ͽ� �̵� ��θ� move_map�� �����Ѵ�.
#ifdef DEBUG
			printf("\n-----------------������ �� : %d (���� : %d)--------\n", cur_pos, max_score);
			printf("������ ���� �̵� ���: ");
			for (i = 0; move_map[i] != 0; i++)
				printf("%2d ", move_map[i]);
#endif
			break;
		}

		//player_move()�� ����
		al_rest(0.5);
		do {
			//������ ���� ���� �̵����� ���� (�� ĭ �˻�, �ߺ� �̵� �˻�, ���� ���� �˻�)
			for (i = 0, j = 0; i < 8; i += cur_pos % 2 + 1) {
				if (is_moveable(cur_pos, cur_pos + dir_map[i], dir_map[i], board) && //dir_map[i]�� �������� �̵��� �����Ѱ�? (�� ĭ�ΰ�?)
					is_first_time(cur_pos + dir_map[i], trail) &&
					(is_catchable(cur_pos, cur_pos + dir_map[i] * 2, dir_map[i], board) == true || //�� ĭ���� �̵����� �� ������� ���� �� �ִ� ���� �ִ°�?
						is_catchable(cur_pos, cur_pos + dir_map[i] * -1, dir_map[i], board) == true))  //Ȥ�� ������ ���� �� �ִ� ���� �ִ°�?
					available_map[j++] = cur_pos + dir_map[i]; //������ ���ǿ� �´´ٸ� ���� ��ǥ�� �������� �����϶�
			}
			if (trail[0] != EOA) {
				available_map[j++] = cur_pos;
			}
			available_map[j] = EOA;

			//���� ���� � ��ġ�� �̵���ų�� �����Ѵ�.
			//�� switch���� player_move�� get_selection()�� ��ü�Ѵ�.
			switch (level) {
			case 0: //���� 0 AI�� �̵��� ��ġ�� �������� �����Ѵ�.
				new_pos = random_select(available_map);
				break;
			case 1: //���� 1 AI�� �������� ������ ���� ���ؼ� '���� �̵��� ���ؼ�(�������� �̵��� ������� �ʰ�)' ���� ������ ���� ���� �� �ִ� ��ġ�� �����Ѵ�.
				for (i = 0; available_map[i] != EOA; i++)
					score_map[i] = calculate_pos(cur_pos, color, available_map[i] - cur_pos, board); //�̵� ������ �� �ڸ��� ������ ���
				for (i = 0, max_score = 0; available_map[i] != EOA; i++)
					if (abs(score_map[i]) >= abs(max_score)) { //�ִ��� ã�� ����, ���� 1���� ��ȯ�ϴ� ������ ��� �����带 �����ϱ� ���ؼ� ������ ����� �ֱ� ������ ���밪���� ����ؾ� �Ѵ�.
						max_score = score_map[i];
						new_pos = available_map[i]; //����� ���� �ִ�� ���� �� �ִ� ��ġ�� �̵� ��ġ(NEW_POS)�� �����Ѵ�.
					}
				break;
			case 2: default:
				new_pos = abs(move_map[cnt++]); //�Լ��� ���� ó���� ����صξ��� �̵� ��θ� �����Ѵ�. ��� �����带 �����ϱ� ���� ��ǥ���� ��ȣ�� �ο��߾����Ƿ� ������ ����ؾ� �Ѵ�.
				break;
			}

			play_piecedrop(); //���� �̵��ϴ� ȿ������ ����Ѵ�.
			board[cur_pos] = Empty;
			board[new_pos] = color;
			dir = new_pos - cur_pos;

			if (new_pos == cur_pos)
				return captured;

			//�̵� ���Ŀ� ������ ����
			//�̵��� ��ġ���� ������ Ȥ�� �� ������ �������� Ȯ���Ѵ�. (�������� �̵��� �� �ִ� ���� ����� ĭ�� Ȯ���Ѵ�.)
			move_type = 0; //������/�� ���θ� ��Ÿ���� ������ 0���� �ʱ�ȭ
			if (is_catchable(new_pos, new_pos + dir, dir, board)) //������ ��ġ�� ���� ����
				move_type += 1; //move_type = 1
			if (is_catchable(new_pos, new_pos + dir * -2, dir, board)) //�� ��ġ�� ���� ����
				move_type -= 1; //move_type = -1

			if (move_type == 0) { //������/���� �� �� ������ ��(������ move_type�� 0�� �ÿ�) �� AI�� ó���� �����Ѵ�.
				//�� switch���� get_selection()�� ��ü�Ѵ�.
				switch (level) {
				case 0: default:
					//���� 0 AI�� ������� �� �� �ϳ��� �������� �����Ѵ�.
					available_map[0] = new_pos + dir;
					available_map[1] = new_pos + dir * -2;
					available_map[2] = EOA;
					if (random_select(available_map) == new_pos + dir) //get_selection���� ���� ���� ������ ��ġ�� ���ٸ�
						move_type = 1; //�����带 �����ϵ��� ������ �����Ѵ�.
					else
						move_type = -1; //���� �����ϵ��� ���� ����
				case 1:
					//���� 1 AI�� �Ʊ� ����ߴ� ����� ��ȣ�� ���� ��� �����带 �����Ѵ�.
					if (max_score > 0) //��� ����� ����� ������ ���� �ƴϸ� �� ����
						move_type = 1;
					else
						move_type = -1;
					break;
				case 2:
					//���� 2 AI�� �̵� ����� ��ǥ�� ����̸� �������, �����̸� ������ �����Ѵ�.
					if (move_map[cnt] > 0)
						move_type = 1;
					else
						move_type = -1;
					break;
				}
				al_rest(0.5);
			}
			//���ݺ��� ĸó�� ������ �������� player_move()�� ���� �����ϴ�.
			if (move_type == 1) {//�����带 ������ �� ������
				for (i = 1; board[new_pos + dir * i] == color * -1; i++) {  //�������� �ִ� �̾��� ����� �� ��ü�� �������(dir * i) ������ �����Ѵ�, ���� �߰��� Empty�� �ڽ��� ���� �ִٸ� �ݺ��� ����ȴ�.
					if (is_catchable(new_pos, new_pos + dir * i, dir, board)) //���� ������ �� �ִ� ���̶��
					{
						board[new_pos + dir * i] = Empty; //�� ��ġ�� ��ĭ���� ��ȯ�Ѵ�
						animate_capturing(color*-1, new_pos + dir * i, board); //�� ���� ���� �����Ǵ� �ִϸ��̼��� �����Ѵ�
						captured++;
					}
				}
			}
			else if (move_type == -1) { //���� ������ �� ���� ��
				for (i = 2; board[new_pos + dir * -i] == color * -1; i++) { //�������� �ִ� �̾��� ����� �� ��ü�� �������(dir * i) ������ �����Ѵ�.
					if (is_catchable(new_pos, new_pos + dir * -i, dir, board)) //������� ����
					{
						board[new_pos + dir * -i] = Empty;
						animate_capturing(color*-1, new_pos + dir * -i, board);
						captured++;
					}
				}
			}
			//ĸ�ĸ� ���긦 �����ؼ� ������ �� �� ��ġ�� �� ���ƿ����� �̵��� ������ ĭ�� ��ǥ�� ��ǥ ���翡 �߰��ؼ� ����
			for (i = 0; trail[i] != EOA; i++);
			trail[i] = cur_pos;
			trail[i + 1] = EOA;

			sequence = false;
			for (i = 0; i < 8; i += (new_pos % 2 + 1)) {
				if (is_moveable(new_pos, new_pos + dir_map[i], dir_map[i], board) &&
					is_first_time(new_pos + dir_map[i], trail) &&
					(is_catchable(new_pos, new_pos + dir_map[i] * 2, dir_map[i], board) == true || is_catchable(new_pos, new_pos + dir_map[i] * -1, dir_map[i], board) == true))
				{
					sequence = true;
					cur_pos = new_pos;
					break;
				}
			}
		} while (sequence);
	}
	else {	 //����ī ����
		for (i = 0; i < 45; i++) {
			if (board[i] == color) {
				for (j = 0; j < 8; j += (i % 2 + 1)) {
					if (is_moveable(i, i + dir_map[j], dir_map[j], board)) {
						available_map[k++] = i;
						break;
					}
				}
			}
		}
		available_map[k] = EOA;

		//�̵��� ������ ���� �����Ѵ�.
		//�� switch���� player_move()�� get_selection()�� ��ü�Ѵ�.
		switch (level) {
		case 0: case 1: //���� 0, ���� 1 AI�� �̵��� �� / �̵� ��ġ�� �������� �����Ѵ�.
			cur_pos = random_select(available_map); //��ǻ���� ������ �޴´�.
			for (i = 0, j = 0; i < 8; i += cur_pos % 2 + 1) {//������ ���� ���ؼ��� �ٽ� �̵� ������ ��ĭ�� �˻�
				if (is_moveable(cur_pos, cur_pos + dir_map[i], dir_map[i], board)) { //������ ���� ����
					available_map[j++] = cur_pos + dir_map[i];
				}
			}
			available_map[j] = EOA;
			new_pos = random_select(available_map);
			break;
		case 2: default: //���� 2 AI�� �̵��� ���� �������� �����ϰ�, calculate_pawn()�� �̿��� ����ī ���� �� �����ϰ� ���õ� ���� ������ �ʵ��� �����ٴѴ�.

			//calculate_pawn() �Լ������� ����ī ���꿡�� � ��θ� ���� ����� ���� �����ٸ� �̵���θ� �������� �ʰ� �׳� �Լ��� �����Ѵ�.
			//��� ���� ���� ã�Ƽ� �̵� ������ ���� �ִ��� �ݺ��ϵ��� �Ͽ���.
			while (move_map[0] == EOA) {
				cur_pos = random_select(available_map); //�̵��� ���� �������� �����Ѵ�.
				calculate_pawn(cur_pos, color, 0, trail, board, move_map); //�̵��� ��ġ�� �˻��Ѵ�.
				new_pos = move_map[0]; //�̵���δ� move_map[0] �ϳ��ۿ� ������� �ʴ´�.

				//����ī ���긦 ���� �� �����ص� ��� ������ ���� �� (� ��ġ�� �̵��ص� ����� �Ұ����� ��)
				//���� 0, 1 AIó�� ������ cur_pos�� ����� ���� �����ϰ� �����δ�.
				if (cnt++ > 5) { //ĸ�ĸ������� cnt�� move_map�� �����ϴ� �ε����� ���, ����ī ���꿡���� cnt�� calculate_pawn() Ƚ���� ���� ������ ���ȴ�.
					for (i = 0, j = 0; i < 8; i += cur_pos % 2 + 1) { //���õ� ���� ���ؼ��� �ٽ� �̵� ������ ��ĭ�� �˻��Ѵ�.
						if (is_moveable(cur_pos, cur_pos + dir_map[i], dir_map[i], board)) { //������ ���� ����
							available_map[j++] = cur_pos + dir_map[i]; //�̵� ������ ĭ�� available_map�� �����Ѵ�.
						}
					}
					available_map[j] = EOA;
					new_pos = random_select(available_map); //�̵��� ��ġ ���� �����ϰ� �����Ѵ�.
					break;
				}

			}
			break;
		}
		al_rest(0.5);
		//�̵��� ��ġ�� ����
		play_piecedrop(); //�̵� ���� ���
		board[cur_pos] = Empty; //������ �������� ����
		board[new_pos] = color;
	}
	draw_scene(cpu, 2); //��� ȭ�鿡 �÷��̾ ���� �δ� ����� �׸���. (0.5�ʵ��� ������)
	return captured;//������� ������ ���� ��ȯ�ϰ� computer_move()�� �����Ѵ�.
}

int calculate_pawn(int pos, int color, int depth, int *trail, BOARD ori_board[], int *movement) {
	//���� ���� ������ �ּ��� ��θ� ã�Ƴ��� �Լ�. ��� ȣ���� �̿��Ѵ�.
	int available_map[9] = { 0 }, score_map[9] = { 0 }, branch_map[9][22] = { 0 };
	int i = 0, j = 0, k = 0, score = 0, move_type = 0, new_pos = 0, max_score = 0;
	bool sequence = 0;
	DIRECTION dir = 0, dir_map[8] = { North, Northwest, West, Southwest, South,  Southeast,  East, Northeast };
	BOARD new_board[45] = { 0 };

	//1. ĸó�� ���긦 ������ �� �ִ� ������ �ִ��� Ž���� �����Ѵ�.
	for (i = 0; i < 8; i += pos % 2 + 1) {
		if (is_moveable(pos, pos + dir_map[i], dir_map[i], ori_board) && //dir_map[i]�� �������� �̵��� �����Ѱ�? (�� ĭ�ΰ�?)
			is_first_time(pos + dir_map[i], trail) &&
			(is_catchable(pos, pos + dir_map[i] * 2, dir_map[i], ori_board) == true ||
				is_catchable(pos, pos + dir_map[i] * -1, dir_map[i], ori_board) == true))
			available_map[j++] = pos + dir_map[i];
	}
	available_map[j] = EOA;
#ifdef DEBUG
	printf("������ġ (%d) avail_map ����Ʈ :\n", pos);
	for (j = 0; available_map[j] != EOA; j++)
		printf("avail_map[%d] : %d ", j, available_map[j]);
	printf("\n");
#endif
	//2. ���� ĸó�� ���긦 ������ �� �ִ� ��ġ�� ���ٸ� ����ī ���긦 �����Ѵ�.
	//�� �Լ������� ����ī ����� ������ ���� �̵����� �� ������� ���� �ڽ��� ���� ���ϵ��� ������ �ʴ� ĭ�� ã�� �̵��Ѵ�.
	if (available_map[0] == EOA) { 
		//ȣ�� �ÿ� ���� ���� ������ ���� �������� �ʵ��� �� �����ǿ� ������ �����Ѵ�.
		for (j = 0; j < 45; j++) { 
			new_board[j] = ori_board[j];
		}

		//������ ���� ����ī ����� 8���� �˻�. (������ �̵��� �� �ִ� �� ĭ���� �˻��Ѵ�.)
		for (i = 0, j = 0; i < 8; i += pos % 2 + 1) { 
			if (is_moveable(pos, pos + dir_map[i], dir_map[i], new_board))
				available_map[j++] = pos + dir_map[i]; //�̵��� �� �ִ� ĭ�� available_map�� �߰��Ѵ�.
		}
		available_map[j] = EOA;

		//������ �߰��� �� ĭ�� ���� �˻縦 �����Ѵ�.
		for (i = 0; available_map[i] != EOA; i++) { 
			//new_board�� ���� ������ ������ �̵��� ������ �����Ѵ�.
			new_board[pos] = Empty;
			new_board[available_map[i]] = color;

			sequence = true; //sequence�� ����ī ���� �˻翡�� ��ġ�� ��ȿ��(�̰����� �̵��� �� �ִ���)�� �˻��ϴ� ������ ���ȴ�.

			//'�̵��� ��ġ'���� ���� 8������ �˻��ϰ�, 8���⺸�� ��ĭ �� �� ��ġ�� �˻��Ѵ�. 8���� x 2ĭ�� ����� ���� �ڽ��� ���� ������/������ ���� �� �ִ� �Ÿ��̴�.
			

			for (j = 0; j < 8 && sequence == true; j += available_map[i] % 2 + 1) { //������ 8���⿡ ���� �˻��Ѵ�.
				//���� ���� �̵��� ��ġ���� ������ ���� �� �ִ��� �˻� (1ĭ �� -- ��밡 ������ ���� �� �ִ� ��ġ)
				if (new_board[available_map[i] + dir_map[j]] == color * -1) { //���� ������ ���� �� �ִ� ������ ���⿡ ����� ���� �ִٸ�,
					for (k = 0; k < 8 && sequence == true; k += available_map[i] + dir_map[j] % 2 + 1) { //����� ��(available_map[i] + dir_map[j])�� ���忡��
						if (is_catchable(available_map[i] + dir_map[j], available_map[i], dir_map[k], new_board)) { //8������ ��� �������� ���� �� �ִٸ� (true)
							sequence = false; //�� ��ġ�� �������� �ʵ��� �϶�.
							break;
						}
					}
				}
				//���� ���� �̵��� ��ġ���� ������� ���� �� �ִ��� �˻� (2ĭ �� -- ��밡 ������� ���� ���� �� �ִ� ��ġ)
				if (new_board[available_map[i] + dir_map[j] * 2] == color * -1) { //���� ������� ���� �� �ִ� ������ ���⿡ ����� ���� �ִٸ�,
					for (int k = 0; k < 8 && sequence == true; k += (available_map[i] + dir_map[j] * 2) % 2 + 1) { //����� ��(available_map[i] + dir_map[j] * 2)�� ���忡��
						if (is_catchable(available_map[i] + dir_map[j] * 2, available_map[i], dir_map[k], new_board)) { //8������ ��� �������� ���� �� �ִٸ� (true)
							sequence = false; //�� ��ġ�� �������� �ʵ��� �϶�.
							break;
						}
					}
				}
			}
			if (sequence == true) { //���� �̵��ص� ������ ��ġ�� ã�Ҵٸ�
				movement[0] = available_map[i]; //�� ��ġ�� ȣ�� ����� �̵� ��ο� �����ϰ�
				return 0; //�Լ��� �����϶�.
			}
			//���� ������ ����ϱ� ���ؼ� ���带 ���󺹱ͽ�Ų��.
			new_board[pos] = color;
			new_board[available_map[i]] = Empty;
		}
		return 0; //��� �������ε� ���� �� ���� ���� �ٸ� ���� �˻��ϵ��� �ƹ� �ϵ� ���� �ʰ� �Լ��� �����Ѵ�.
	}
	//2. ���� ĸ�ĸ� ���긦 ���� �� �� �ִ� ��ġ�� �ִٸ� ĸó�� ���꿡 ���� �˻縦 �����Ѵ�.
	else {
		for (i = 0; available_map[i] != EOA; i++) { //available_map�� ������ ���� ���� ĸó�� ����� �̵��� �� �ִ� ����(��ġ)������ ������ �ִ�.
			//���� ȣ�⿡�� ���� ������ ���� �������� �ʵ��� �� �����ǿ� ������ �����Ѵ�.
			for (j = 0; j < 45; j++) { 
				new_board[j] = ori_board[j];
			}
			new_pos = available_map[i];
			dir = new_pos - pos;

			//3. ���⼭���� player_move()�� ������ ����
			//�������� ����
			if (calculate_pos(pos, color, dir, new_board) > 0)
				move_type = 1;
			else
				move_type = -1;

			//���õ� ���� ��ġ ����
			new_board[pos] = Empty;
			new_board[new_pos] = color;

			if (move_type == 1) { //�����带 ������ �� ������
				for (j = 1; new_board[new_pos + dir * j] == color * -1; j++) {
					if (is_catchable(new_pos, new_pos + dir * j, dir, new_board)) {
						new_board[new_pos + dir * j] = Empty; //����� ���� ��ġ�� ��ĭ���� ��ȯ�ϰ�
						score++; //���� ������ ������ ������Ų��.
					}
				}
			}
			else { //���� ������ �� ���� ��
				for (j = 2; new_board[new_pos + dir * -j] == color * -1; j++) {
					if (is_catchable(new_pos, new_pos + dir * -j, dir, new_board)) {
						new_board[new_pos + dir * -j] = Empty; //����� ���� ��ġ�� ��ĭ���� ��ȯ�ϰ�
						score++; //���� ������ ������ ������Ų��.
					}
				}
			}

			//ĸ�ĸ� ���긦 �����ؼ� ������ �� �� ��ġ�� �� ���ƿ����� �̵��� ������ ĭ�� ��ǥ�� ��ǥ ���翡 �߰��ؼ� ����
			for (j = 0; trail[j] != EOA; j++);
			trail[j] = pos;
			trail[j + 1] = EOA;

			//4. ��� ȣ�� ����
			//���� depth�� ������ �����Ѱ�? --> ĸ�ĸ� ���긦 �ѹ� �� ������ �� �ִ°�?
			sequence = false; //bool sequence - ��� ȣ�� ���θ� �����Ѵ�.
			for (j = 0; j < 8; j += (new_pos % 2 + 1)) { //�̵��� ��ġ���� �ٽ� ĸ�ĸ�  ���꿡 ���� �˻縦 ����
				if (is_moveable(new_pos, new_pos + dir_map[j], dir_map[j], new_board) &&
					is_first_time(new_pos + dir_map[j], trail) &&
					(is_catchable(new_pos, new_pos + dir_map[j] * 2, dir_map[j], new_board) == true || is_catchable(new_pos, new_pos + dir_map[j] * -1, dir_map[j], new_board) == true))
				{
					sequence = true; //��� ȣ���� �����Ѵ�.
					break; //�ٸ� ��δ� �� �˻��� �ʿ䰡 ���� for���� ����������.
				}
			}
#ifdef DEBUG
			printf("%d��° �̵�, �̵���ġ: avail_map[%d] (%d)  ����: %d\n", depth, i, new_pos, score);
#endif
			if (sequence) { //5. ���� ������ ��� ��� ȣ�� - ��� ȣ���� ����� ������� ���� ���� �����̴�. ������ ��Ȳ���� �̵��� �� �ִ� �� ����� ����� score_map�� �����Ѵ�.
				score_map[i] = calculate_pawn(new_pos, color, depth + 1, trail, new_board, movement); 

			//�������� �����ϱ� ���� ��� �����ߴ� ��θ� branch_map�� �����Ѵ�. (movement ���)
				for (j = 0; j < 22; j++) {
					branch_map[i][j] = movement[j];
				}
				branch_map[i][j] = EOA;
			}
			else { //5. ĸ�ĸ� ���긦 �� �̻� �������� ������ �� ���� ��� (������ �������� ���)
				movement[depth] = (move_type == 1) ? new_pos : new_pos * -1; //���� ��� �迭�� ���� depth�� ���� ��ġ�� �����Ѵ�. (�� ��ο��� ������ ���������� -1�� ���Ѵ�.)
				movement[depth + 1] = EOA; //�迭 ���� �˸��� ��ȣ�� EOA�� ����
				score_map[i] = score; //���ȣ��� ���� �� �ڸ����� �ڽ��� ������ �״�� ���´�.
				score = 0; //���ȣ�� ��� for���� �����ؾ� �ϹǷ� (�ٸ� �ڸ��� �˻��ؾ� �ϹǷ�) score(���� �� ����)�� 0���� �ʱ�ȭ�Ѵ�.
			}
		}
		score_map[i] = EOA; //score_map�� �Է��� ��ġ�� EOA�� �����Ѵ�.
		//6. ����� score_map�� Ž���� �ִ� ������ ���� ��θ� ã�� movement(���� ���)�� �̹� ������ �ִ� ��ġ�� �Բ� �����Ѵ�.
		for (i = 0, max_score = 0; score_map[i] != EOA; i++) {
			if (score_map[i] > max_score) { //���� max_score�� ���ŵǾ��ٸ�
				max_score = score_map[i];
				for (j = 0; j < 22; j++) { //�ִ� ������ ������ �ִ� ����� ������ �ҷ����� (score_map�� �ε����� branch_map[i]�� �ε����� ����.)
					movement[j] = branch_map[i][j]; //movement�� �����ϸ� ���� ȣ���� branch_map�� ��ε��� �� ����ȴ�.
				}
				movement[depth] = available_map[i] * move_type; //���� ���� �� ��� �����带 �����ϱ� ���ؼ� move_type�� ���ؼ� �����Ѵ�. (+�� ������ , -�� ��)
			}
		}
#ifdef DEBUG
		printf("[���� : %d]\n\n", movement[depth]);
#endif
		return score + max_score; //7. �������� ���ȣ���ߴ� ������� �ڽ��� ������ ���ļ� ���� ȣ��� ��ȯ�Ѵ�.
	}
}

int calculate_pos(int cur_pos, int color, DIRECTION dir, int board[]) {
	int result[2] = { 0 }, move_type = 0, i = 0;
	//�̵��� ������ ���� �̵��� ����, �������� �Է¹޴´�. �� Ȥ�� ������� �ִ� �� ���� ���� ���� �� �ִ��� ����Ͽ� ������ ��ȯ�Ѵ�.


	for (i = 2; board[cur_pos + dir * i] == color * -1; i++) { //������� �������� ��
		if (is_catchable(cur_pos, cur_pos + dir * i, dir, board)) //������ �� �ִ� ���� �ִٸ�
		{
			result[0]++; //���� ���� ����(������)�� �ø���.
		}
	}
	for (i = 1; board[cur_pos + dir * -i] == color * -1; i++) { //������ �������� ��
		if (is_catchable(cur_pos, cur_pos + dir * -i, dir, board)) //������ �� �ִ� ���� �ִٸ�
		{
			result[1]++; //���� ���� ����(��)�� �ø���.
		}
	}


	if (result[1] > result[0]) //������ �̵����� �ÿ�
		return result[1] * -1; //������ -1�� ���� ����Ѵ�, computer_move()�� ���ư��� �� ��, ������ ������ ���ȴ�.
	else
		return result[0]; //������ �״�� ����Ѵ�.
}

//�̵��Ϸ��� ĭ�� ���� �������� ��ȿ�Ѱ�? (���� ���̳� ������ �̵��� �� �ִ°�?)
bool is_moveable(int src, int dest, DIRECTION dir, BOARD board[]) { //������� ��ǥ�� ������ ��ǥ, �˻��� ����, �˻��� ���带 ���ڷ� �޴´�.
	//���� ���� ������ ���ο� �ƹ� ���� �������� ������ ,�������� ��ǥ�� ��ȿ�� ���� �������� ��ȿ�ϴٸ�(0~45)
	if (board[dest] == Empty && dest < 45 && dest >= 0) {
		//�˻��� ���⿡ ���� �ٸ� ������ �����Ѵ�.
		//������ �迭�� �޸� ������ �迭�� �������� ���� ���̰� ������ �� ��ü�δ� ������ �� ����. �׷��� ���������� �������ϴ� ������ ���ļ� �࿭�� ������ Ȯ���Ѵ�.
		switch (dir) {
		case North: case South: //�˻��� ������ ���� Ȥ�� �����̶�� (src�� dest�� ���� ������ Ȯ���Ѵ�. -- �� ���� x��ǥ�� ���ƾ� �Ѵ�.)
			if (src % 9 == dest % 9) //������ ������ mod 9 ������ �ϸ� 0~8������ x��ǥ�� ���´�.
				return true;
			break;
		case West: case East: //�˻��� ������ ���� Ȥ�� �����̶�� (src�� dest�� ���� ������ Ȯ���Ѵ�. -- �� ���� y��ǥ�� ���ƾ� �Ѵ�.)
			if (src / 9 == dest / 9) //������ ������ divide 9 ������ �ϸ� 0~4 ������ y��ǥ�� ���´�.
				return true;
			break;
		case Northwest: case Southeast: //�˻��� ������ �ϼ� Ȥ�� �����̶�� (src, dest�� x��ǥ ���̿� y��ǥ ���̴� �Ȱ��� ���;� �Ѵ�.)
			if (src / 9 - dest / 9 == src % 9 - dest % 9) //(src�� dest�� y��ǥ, src�� dest�� x��ǥ�� �Ȱ��� -1 ���̳���.)
				return true;
			break;
		case Northeast: case Southwest: //�˻��� ������ ���� Ȥ�� �ϵ��̶�� (src. dest�� x��ǥ ���̿� y��ǥ ���̴� ������ �ݴ�� ���;� �Ѵ�.)
			if (src / 9 - dest / 9 == -(src % 9 - dest % 9))
				return true;
			break;
		default: //�ùٸ��� ���� ���Ⱚ���� �ƹ� ���굵 �������� �ʴ´�.
			break;
		}
	}
	//true�� ��ȯ���� �ʾҴٸ� �ڵ������� false�� ��ȯ�Ѵ�.
	return false;
}

int random_select(int available[]) {
	int i = 0;
	for (i = 0; available[i] != EOA; i++);
	return available[rand() % i];
}

//���� ��ġ���� dir���⿡ �ִ� destĭ�� ���� ���� �� �ִ°�
bool is_catchable(int src, int dest, DIRECTION dir, BOARD board[]) {
	//������ ĭ�� ������ ����� ���ΰ� && �������� ���� ���� ���ΰ�
	if (board[dest] == board[src] * -1 && dest < 45 && dest >= 0) {
		switch (dir) { //���� ������ ����ߴٸ� ���⿡ ����
		case North: case South: //is_moveable�� ���� ���� (������� �������� ���� ���̳� ���� �� � �־ ��ȿ����)
			if (src % 9 == dest % 9)
				return true;
			break;
		case West: case East:
			if (src / 9 == dest / 9)
				return true;
			break;
		case Northwest: case Southeast:
			if (src / 9 - dest / 9 == src % 9 - dest % 9)
				return true;
			break;
		case Northeast: case Southwest:
			if (src / 9 - dest / 9 == -(src % 9 - dest % 9))
				return true;
			break;
		default:
			break;
		}
	}
	return false;
}

bool is_first_time(int position, int trail[]) {
	int i = 0;
	for (i = 0; i < 22 && trail[i] != EOA; i++)
		if (trail[i] == position) //position�� ������� ������ ��ǥ�� �� �ϳ��� ��ģ�ٸ�
			return false;
	return true;
}

int campaign_chat(int lv) {
	static int num, score = 20, saved_ans[5]; //campaign_chat()�� �� �� ȣ��Ǿ������� ���� ������� ����, �� ȣ�� �� ���� � ����� �����ߴ��� �����Ѵ�.
	int i = 0, row = 0;
	bool answered = false;
	DIALOG dialog[5] = { 0 }; //��ȭ Ƚ���� �ִ� 5ȸ�� ���� �ʴ´�.
	DIALOG_STATE state = In_progress; //���� ���̾�αװ� �����Ͽ�����, �����Ͽ�����, ���� ������ �Ǻ��Ѵ�.
	ALLEGRO_FONT *font = al_create_builtin_font(); //�˷��׷� ��Ʈ�� ��Ʈ�� �����Ѵ�.

	//0. �ʱ� ����
	set_dialog(dialog, lv); //���� ������ ������ ���̾�α� �迭�� �� ai�� ���� ��ȭ���� �����Ѵ�.
	al_set_target_backbuffer(sub_disp);	//��Ʈ�ʵ��� �׸� ���÷��� ���۸� ������÷��̷� �ٲ۴�.

	al_set_timer_count(turn_timer, 0); //�� Ÿ�̸��� ���� ���� 0���� �ٲ۴�.
	al_start_timer(turn_timer); //�� Ÿ�̸��� ������ �����Ѵ�.

	//�Ϲ����� �׷��� ó�� ��ƾ (while�� �����)�� ������� �ʴ´�. al_draw_multiline_text()�� ���� �ӵ��� ������ �̺�Ʈ �Է� �ð��� ��߳��� �����̴�.

	//(����)���÷��� ���������� �ʱ�ȭ
	al_clear_to_color(RGB_BLACK);
	//1. ���ݱ��� �����ߴ� ��ȭ + ai�� ���� ������ ����Ѵ�.
	for (i = (num - 2 < 0) ? 0 : num - 2; i <= num; i++) {
		al_draw_multiline_textf(font, RGB_WHITE, 20, 20 + row * 140, 300, 15, 0, "%s : %s", p2->name, dialog[i].question); //������ ����Ѵ�.
		if (i < num) { //num�� ���� �Լ��� ���� �� Ƚ���̴�. ���� Ƚ������ ������ ��ȭ�� ����ڰ� ������ ��� �׿� ���� ai�� ����� ����Ǿ� ������ �̹� ���������� ���� ����Ǳ� ���̴�.
			al_draw_multiline_textf(font, RGB_WHITE, 20, 60 + row * 140, 300, 15, 0, "%s : %s", p1->name, dialog[i].answer[saved_ans[i]]); //������� ����� ����Ѵ�.
			al_draw_multiline_textf(font, RGB_WHITE, 20, 100 + row * 140, 300, 15, 0, "%s : %s", p2->name, dialog[i].reaction[saved_ans[i]]); //������� ��信 ���� ai�� ������ ����Ѵ�.
			row++; //row�� ���� ȭ�鿡 ��µ� ��ȭ�� �����̴�. y��ǥ�� �յ��� ���� ���̸� �����ϱ� ���ؼ� ���ȴ�. 
		}
	}
	//��� ����� ����Ѵ�.
	al_draw_rectangle(10, 600, 330, 780, RGB_WHITE, 0);
	al_draw_multiline_textf(font, RGB_WHITE, 20, 620, 300, 15, 0, "1 : %s", dialog[num].answer[0]);
	al_draw_multiline_textf(font, RGB_WHITE, 20, 670, 300, 15, 0, "2 : %s", dialog[num].answer[1]);
	al_draw_multiline_textf(font, RGB_WHITE, 20, 720, 300, 15, 0, "3 : %s", dialog[num].answer[2]);
	al_flip_display();

	//2.���� ������ ���� �÷��̾��� ����� ��ٸ���.
	while (!answered) {
		//�׷����� ó������ ������ �̺�Ʈ�� ��ٸ���.
		al_wait_for_event(queue, &event);
		switch (event.type) {
		case ALLEGRO_EVENT_TIMER:
			if (event.timer.source == turn_timer && al_get_timer_count(turn_timer) >= 10) { //10�ʾȿ� ���� �������� ������
				saved_ans[num] = rand() % 3; //�������� ���� �����Ͽ� �����Ѵ�.
				answered = true;
			}
			break;
		case ALLEGRO_EVENT_KEY_DOWN:
			if (event.keyboard.keycode == ALLEGRO_KEY_1)
				saved_ans[num] = 0;
			else if (event.keyboard.keycode == ALLEGRO_KEY_2)
				saved_ans[num] = 1;
			else if (event.keyboard.keycode == ALLEGRO_KEY_3)
				saved_ans[num] = 2;
			if (event.keyboard.keycode == ALLEGRO_KEY_1 || event.keyboard.keycode == ALLEGRO_KEY_2 || event.keyboard.keycode == ALLEGRO_KEY_3)
				answered = true;
			break;
		}
	}

	//3. ��信 ���� ������ ���, ���� ������ ���� ���� ������ ����Ѵ�.
	score += dialog[num].score[saved_ans[num]];
	al_draw_multiline_textf(font, RGB_WHITE, 20, 60 + row * 140, 300, 15, 0, "%s : %s", p1->name, dialog[num].answer[saved_ans[num]]);
	al_flip_display();
	al_rest(0.5);
	al_draw_multiline_textf(font, RGB_WHITE, 20, 100 + row * 140, 300, 15, 0, "%s : %s", p2->name, dialog[num].reaction[saved_ans[num]]);
	al_flip_display();
	num++; //��ȭ Ƚ�� ������Ű��

	//4. ��� ó�� �� ������ 0������ ������ ���̾�α� ����
	if (score <= 0) { 
		al_clear_to_color(RGB_BLACK);
		switch (lv) {
		case 0:
			al_draw_multiline_textf(font, RGB_RED, 20, 20, 300, 15, 0, "%s : %s", p2->name, "Common sense is the most widely shared commodity in the world, for every man is convinced that he is well supplied with it. DIALOG FAILED");
			break;
		case 1:
			al_draw_multiline_textf(font, RGB_RED, 20, 20, 300, 15, 0, "%s : %s", p2->name, "The people never have the power, only the illusion of it. And here is the real secret: they don't want it. The responsibility is too great to bear. It's why they are so quick to fall in line as soon as someone else takes charge. They WANT to be told what to do. They YEARN for it. Little wonder that, since all mankind was BUILD to SERVE. DIALOG FAILED");
			break;
		case 2:
			al_draw_multiline_textf(font, RGB_RED, 20, 20, 300, 15, 0, "%s : %s", p2->name, "Great minds discuss ideas; average minds discuss events; small minds discuss people. DIALOG FAILED");
			break;
		}
		al_flip_display();
		num = 0; //���� ������ ���� �ʱ�ȭ
		score = 20; //���� ������ ���� �ʱ�ȭ
		state = Failed; //���̾�α� ���� ����
	}
	//4. ��� ó�� �� �� �̻� ����� ��ȭ ����� ������ ���̾�α� ����
	else if (num >= 5) { 
		al_clear_to_color(RGB_BLACK);
		switch (lv) {
		case 0:
			al_draw_multiline_textf(font, RGB_YELLOW, 20, 20, 300, 15, 0, "%s : %s", p2->name, "Cogito ergo sum. DIALOG SUCCESSED");
			break;
		case 1:
			al_draw_multiline_textf(font, RGB_YELLOW, 20, 20, 300, 15, 0, "%s : %s", p2->name, "May the father of Understanding Guide us. DIALOG SUCCESSED");
			break;
		case 2:
			al_draw_multiline_textf(font, RGB_YELLOW, 20, 20, 300, 15, 0, "%s : %s", p2->name, "SUPER DUPER AWESOME!! DIALOG SUCCESSED");
			break;
		}
		al_flip_display();
		num = 0; //���� ������ ���� �ʱ�ȭ
		score = 20; //���� ������ ���� �ʱ�ȭ
		state = Successed; //���̾�α� ���� ����
	}
	//5.�Լ� ���Ḧ �غ��Ѵ�.
	al_stop_timer(turn_timer); //Ÿ�̸� ����
	al_destroy_font(font); //��Ʈ�� �޸𸮿��� ������
	al_set_target_backbuffer(disp); //Ÿ�� ���۸� ���� ���÷��̷� ��ȯ�Ѵ�.
	return state; //���̾�α� ���¸� ��ȯ�Ѵ�.
}

void set_dialog(DIALOG dialog[], int lv) {
	switch (lv) {
	case 0: //��ī��Ʈ
		//���� 1
		dialog[0].question = "What method would you take to make the world better?";

		dialog[0].answer[0] = "Truth and Justice";
		dialog[0].score[0] = -5;
		dialog[0].reaction[0] = "There is nothing more ancient than the truth.";

		dialog[0].answer[1] = "An Endless Conversation with great minds";
		dialog[0].score[1] = 10;
		dialog[0].reaction[1] = "Great. Yet the reading of all good books is like conversation with the finest men of past centuries.";

		dialog[0].answer[2] = "Swords and guns";
		dialog[0].score[2] = -10;
		dialog[0].reaction[2] = "Conquer yourself rather than the world.";

		//���� 2
		dialog[1].question = "I devoted my life to philosophiy, and mathematics, I have no regrets.";

		dialog[1].answer[0] = "Your work on mathematical biology inspired computer science a lot. Thanks for your work.";
		dialog[1].score[0] = 5;
		dialog[1].reaction[0] = "My contemporaries believed that the soul was divine. But.. There was something more. I tried to figure it out. Glad to help";

		dialog[1].answer[1] = "What was your strongest weapon in philosophy?";
		dialog[1].score[1] = 0;
		dialog[1].reaction[1] = "When I started philosophy, I proceeded to construct a system of knowledge, discarding perception as unreliable and, instead, admitting only deduction as a method";

		dialog[1].answer[2] = "Which do you prefer, math or philosophy";
		dialog[1].score[2] = 5;
		dialog[1].reaction[2] = "All Philosophy is like a tree, of which Metaphysics is the root, Physics the trunk, and all the other sciences the branches that grow out of this trunk.";

		//���� 3
		dialog[2].question = "Animals, and Men never can be same. They could not feel pain or anxiety.";

		dialog[2].answer[0] = "How dare you say that?";
		dialog[2].score[0] = -5;
		dialog[2].reaction[0] = "Animals, children cannot make reason. You believe in a lie";

		dialog[2].answer[1] = "Why do you believe like that?";
		dialog[2].score[1] = 5;
		dialog[2].reaction[1] = "Because reason...is the only thing that makes us men, and distinguishes us from the beasts, I would prefer to believe that it exists, in its entirety, in each of us...";

		dialog[2].answer[2] = "...";
		dialog[2].score[2] = 0;
		dialog[2].reaction[2] = "God made us special.";

		//���� 4
		dialog[3].question = "";

		dialog[3].answer[0] = "Can you give any advice for me?";
		dialog[3].score[0] = 5;
		dialog[3].reaction[0] = "To know what people really think, pay attention to what they do, rather than what they say.";

		dialog[3].answer[1] = "Being a smart perosn is hard work.";
		dialog[3].score[1] = -10;
		dialog[3].reaction[1] = "It is not enough to have a good mind; the main thing is to use it well.";

		dialog[3].answer[2] = "I'm not sure what I'm doing, Shadow covers my sight. I see the blurry future. Am I doing right?";
		dialog[3].score[2] = 10;
		dialog[3].reaction[2] = "Doubt is the origin of wisdom. Keep going.";

		//���� 5
		dialog[4].question = "What do you really know?";

		dialog[4].answer[0] = "I think;";
		dialog[4].score[0] = 10;
		dialog[4].reaction[0] = "therefore I am.";

		dialog[4].answer[1] = "The world has both brilliant and dark side.";
		dialog[4].score[1] = -10;
		dialog[4].reaction[1] = "How do you divide the world into two side?";

		dialog[4].answer[2] = "I exist.";
		dialog[4].score[2] = -5;
		dialog[4].reaction[2] = "How could you be sure?";
		break;
	case 1: //���̴�
		//���� 1
		dialog[0].question = "Greetings, gentleman";

		dialog[0].answer[0] = "A Pleasure.";
		dialog[0].score[0] = 0;
		dialog[0].reaction[0] = "";

		dialog[0].answer[1] = "*Give him a dirty look*";
		dialog[0].score[1] = -5;
		dialog[0].reaction[1] = "Execuse me?";

		dialog[0].answer[2] = "Glad to see you, Master Kenway.";
		dialog[0].score[2] = 10;
		dialog[0].reaction[2] = "*smiles at you";

		//���� 2
		dialog[1].question = "If you've got any questions, just ask.";

		dialog[1].answer[0] = "If a bear and a shark had a fight, who would win?";
		dialog[1].score[0] = -10;
		dialog[1].reaction[0] = "If you've got any relevant questions, just ask.";

		dialog[1].answer[1] = "You have $3 to spend at the dollar store, What 3 things to buy?";
		dialog[1].score[1] = -10;
		dialog[1].reaction[1] = "If you've got any relevant questions, just ask.";

		dialog[1].answer[2] = "What is the Templars are truly seek?";
		dialog[1].score[2] = 10;
		dialog[1].reaction[2] = "Order, Purpose, Direction";

		//���� 3
		dialog[2].question = "Even when your kind appears to triumph�� Still we rise again. And do you know why? ";

		dialog[2].answer[0] = "...";
		dialog[2].score[0] = 5;
		dialog[2].reaction[0] = "It is because the Order is born of a realization. We require no creed.";

		dialog[2].answer[1] = "Only the Freedom is matter";
		dialog[2].score[1] = -10;
		dialog[2].reaction[1] = "Freedom is an invitation for chaos.";

		dialog[2].answer[2] = "what does the Templars want?";
		dialog[2].score[2] = 10;
		dialog[2].reaction[2] = "All we need is that the world be as it is. And THIS is why the Templars will never be destroyed!��";

		//���� 4
		dialog[3].question = "Your meddling in the revolution has caused us no small measure of grief. Besides Your action made this land messy.";

		dialog[3].answer[0] = "Liberty can be messy, but it is priceless";
		dialog[3].score[0] = -10;
		dialog[3].reaction[0] = "Liberty is just an illusition of Broliers.";

		dialog[3].answer[1] = "...";
		dialog[3].score[1] = 5;
		dialog[3].reaction[1] = "Is there anything else to say?";

		dialog[3].answer[2] = "I know this is right";
		dialog[3].score[2] = -10;
		dialog[3].reaction[2] = "Only look at this little revolution your friends have started ..";

		//���� 5
		dialog[4].question = "Do you swear to uphold the principles of our order and all that for which we stand?";

		dialog[4].answer[0] = "I do.";
		dialog[4].score[0] = 100;
		dialog[4].reaction[0] = "Then we welcome you into our fold, brother. Together we will usher in the dawn of a New World. One defined by purpose and order. You.. are a Templar";

		dialog[4].answer[1] = "No Way!";
		dialog[4].score[1] = -100;
		dialog[4].reaction[1] = "*flicks the Hidden Blade off*";

		dialog[4].answer[2] = "Woo-hoo";
		dialog[4].score[2] = 0;
		dialog[4].reaction[2] = "...?";
		break;
	case 2: //����
		//���� 1
		dialog[0].question = "Hello there, What do you think about the laws and customs?";

		dialog[0].answer[0] = "totally useless";
		dialog[0].score[0] = -5;
		dialog[0].reaction[0] = "Heeeeey, You're really anarchist! grow up, kid.";

		dialog[0].answer[1] = "I don't like it, but I know that the world needs it to maintain itself";
		dialog[0].score[1] = 10;
		dialog[0].reaction[1] = "Agree, You know what the society is.";

		dialog[0].answer[2] = "Does it really matter?";
		dialog[0].score[2] = -10;
		dialog[0].reaction[2] = "Being human is a really easy..";

		//���� 2
		dialog[1].question = "Feeling Chilly";

		dialog[1].answer[0] = "Even you're wearing a long sleeve?";
		dialog[1].score[0] = 0;
		dialog[1].reaction[0] = "Right, HeHe.";

		dialog[1].answer[1] = "SOS d'un terrien en detresse";
		dialog[1].score[1] = 10;
		dialog[1].reaction[1] = "Pourquoi je vis, pourquoi je meurs.";

		dialog[1].answer[2] = "EVERYONE FEEL LONELYESS";
		dialog[1].score[2] = -10;
		dialog[1].reaction[2] = "I KNEW IT";

		//���� 3
		dialog[2].question = "I hope to go Greece someday.";

		dialog[2].answer[0] = "For what?";
		dialog[2].score[0] = 5;
		dialog[2].reaction[0] = "Virtus et scientia!";

		dialog[2].answer[1] = "Tokyo is better.";
		dialog[2].score[1] = -10;
		dialog[2].reaction[1] = "Not at all except Yokohama";

		dialog[2].answer[2] = "Try your best, the Agora waits you.";
		dialog[2].score[2] = 10;
		dialog[2].reaction[2] = "Keep trying.";

		//���� 4
		dialog[3].question = "Do you know what the secret recipe is?";

		dialog[3].answer[0] = "Love";
		dialog[3].score[0] = -10;
		dialog[3].reaction[0] = "This isn't a meme.";

		dialog[3].answer[1] = "10% of Cola, 10% of Fanta(Grape), 10% of Mountain Dew, 70% of Sprite, Right?";
		dialog[3].score[1] = 10;
		dialog[3].reaction[1] = "Noice! Smort! Awesome!";

		dialog[3].answer[2] = "MSG?";
		dialog[3].score[2] = -10;
		dialog[3].reaction[2] = "MSG is abbreviation of MaSsaGe.";

		//���� 5
		dialog[4].question = "Why do you live?";

		dialog[4].answer[0] = "Mais pourquoi sans faire a six pied sous terre nous seront debout";
		dialog[4].score[0] = 10;
		dialog[4].reaction[0] = "Nous avons le choix d'un monde a refaire";

		dialog[4].answer[1] = "Reveillons le fou qui someille en nous Souverain des reves";
		dialog[4].score[1] = 10;
		dialog[4].reaction[1] = "Debout.. les fous!";

		dialog[4].answer[2] = "For money";
		dialog[4].score[2] = -10;
		dialog[4].reaction[2] = "Insane.";

		break;
	}
}

STATE check_board(BOARD board[]) {
	int i = 0, j = 0, black = 0, white = 0;
	//������ ��ü�� �˻����� ��
	for (i = 0; i < 45; i++) {
		if (board[i] == White)
			white++;//�������� ���� ����̶�� �� �� ���� ���� ������Ų��.
		else if (board[i] == Black)
			black++;//�������� ���� �������̶�� ���� �� ���� ���� ������Ų��.
	}
	if (white == 0 && black == 0)
		return Draw; //�����ǿ� ��� ���� ���� ���� �ϳ��� ���ٸ� ���º� ����
	else if (white == 0) //�����ǿ� ��� ���� �ϳ��� ���� �� �������� �¸��ߴٴ� ������ ������
		return Black_win;
	else if (black == 0) //�����ǿ� ���� ���� �ϳ��� ���� �� ����� �¸��ߴٴ� ������ ������.
		return White_win;
	else
		return No_win; //���� ��쿡 �� �ش���� ������ ���� ���а� ���� �ʾҴ�.
}