//게임 로직에 관련된 함수 &	변수
#include "graphics.h"
#include "main.h"
#include "game.h"
#include "user.h"
#include "audio.h"
#pragma warning(disable:4996)

void init_board(BOARD board[])
{
	int i = 0;
	//상단 0~17, 18, 20, 23, 25 (흑돌) 하단 19, 21, 24, 26, 27~44 (백돌)
	for (i = 0; i < 18; i++) {
		board[i] = Black; //흑돌
	}
	board[18] = board[20] = board[23] = board[25] = Black; //중간 줄
	board[22] = Empty;
	board[19] = board[21] = board[24] = board[26] = White;
	for (i = 27; i < 45; i++) {
		board[i] = White; //백돌
	}
}

void play_game(int mode) {
	int p1_color = 0, lv = 0, i = 0, now_turn = White, congest = 0, captured_pawn = 0;
	DIALOG_STATE dialog_state = In_progress;
	STATE state = No_win;
	BOARD board[45] = { 0 };

	//게임 시작 시 모드에 상관없이 공통적으로 초기화하는 부분
	init_board(board); //보드 초기화
	p1 = &local_user; //현재 게임에서 첫번째 플레이어의 정보를 저장한다.

	turn_timer = al_create_timer(1); //턴 타이머를 생성한다.
	al_register_event_source(queue, al_get_timer_event_source(turn_timer)); //턴 타이머를 이벤트 큐에 등록하여 추후 이벤트를 받을 수 있도록 만든다.

	draw_board(board); //디스플레이 버퍼에 먼저 보드를 그린다.
	draw_scene(NULL, 0); //디스플레이 버퍼에 먼저 상단 화면(배경)을 그린다.

	//캠페인 모드나 프리플레이 모드일 경우 AI의 난이도를 설정한다. && 랜덤함수 사용 준비를 한다.
	if (mode == Campaign || mode == Freeplay) {
		srand((unsigned int)time(NULL)); //랜덤함수 사용을 위한 시드값 준비
		lv = set_cpu_level(); //cpu 레벨을 설정한다.
	}

	//로컬 유저의 선/후공을 결정하고, 결과를 p1_color에 저장한다.
	p1_color = whos_first();

	/*****************************변경점*******************************/
	switch (mode) {
	case Campaign:
		create_sub_display(); //다이얼로그 진행을 위한 서브 디스플레이를 생성한다.

		p2 = &cpu;
		while (state == No_win) { //승부가 나지 않은 동안 아래의 코드를 반복한다.
			if (now_turn == p1_color)
				captured_pawn = player_move(now_turn, p1, board); //플레이어의 턴은 그냥 진행한다.
			else {
				captured_pawn = computer_move(now_turn, p2, lv, board); //컴퓨터의 턴을 진행하는 함수이다.

				//다이얼로그 미니게임
				if ((rand() % 10) < 4 && dialog_state == In_progress) { //컴퓨터의 턴이 끝나면 40% 확률로 다이얼로그 미니게임이 갑자기 진행된다.
					dialog_state = campaign_chat(lv);

					if (dialog_state == Successed) { //다이얼로그 미니게임을 성공적으로 클리어했을 경우 (5회가 지나도 점수가 0 이상일 경우)
						draw_scene(p2, 3); //상대방의 이벤트 컷씬을 보여준다. (상단 화면)
						al_show_native_message_box(NULL, "Fanorona", "Dialog end", "The opponent is satisfied with the great conversation. Dialog event successfully ended. Keep going back to play", NULL, ALLEGRO_MESSAGEBOX_WARN); //네이티브 메시지 박스를 이용해 다이얼로그 미니게임을 클리어했음을 알린다.
						dialog_state = Nomore;
					}
					else if (dialog_state == Failed) { //다이얼로그 미니게임을 실패했을 경우 (5회가 지나기 전에 점수가 0 이하로 떨어졌을 경우)
						al_show_native_message_box(NULL, "Fanorona", "Dialog end", "The opponent is pissed off now by your arrogant words. You lose.", NULL, ALLEGRO_MESSAGEBOX_ERROR); //네이티브 메시지 박스를 이용해 다이얼로그 게임을 실패했음을 알린다.
					//다이얼로그 이벤트 실패 시 메인 게임의 진행과 상관 없이 강제로 보드판에서 플레이어의 말을 삭제하여 다음 check_board()에서 플레이어가 지게 만든다.
						for (i = 0; i < 45; i++) {
							if (board[i] == p1_color)
								board[i] = Empty;
						}
						break;
					}
				}
			}
			if (captured_pawn == 0)
				congest++; //정체 상태 턴 수를 증가시킨다.
			else
				congest = 0;

			if (congest > 15) {//만약 정체 상태인 채로 15턴 이상 지났다면
				if (al_show_native_message_box(NULL, "Fanorona", "Draw", "Wanna end this game?", NULL, ALLEGRO_MESSAGEBOX_YES_NO) == 1) { //네이티브 다이얼로그를 이용해 플레이어(local)에게 무승부를 제안하라
				//제안 수락시 무승부 상태로 판을 업데이트
					for (i = 0; i < 45; i++) //check_board()에서 무승부를 판단하기 위한 조건 -- 모든 말이 보드에 위치하지 않을 것
						board[i] = Empty;
				}
				else {
					//제안 거절시 정체 턴 수를 처음부터 계산한다.
					congest = 0;
				}
			}
			state = check_board(board); //보드판 상태를 체크하고 state 변수에 저장한다.
			now_turn *= -1; //턴을 바꾼다. (말의 색상을 바꾼다.)
		}
		al_destroy_display(sub_disp); //다이얼로그 이벤트용으로 사용한 디스플레이를 삭제한다.
		break;
	case Freeplay:
		p2 = &cpu;
		while (state == No_win) { //승부가 나지 않은 동안 아래 코드를 반복한다.
			if (now_turn == p1_color)
				captured_pawn = player_move(now_turn, p1, board); //플레이어의 턴을 진행
			else
				captured_pawn = computer_move(now_turn, p2, lv, board); //컴퓨터의 턴을 진행
			//여기부터는 Two_player와 같음
			if (captured_pawn == 0)
				congest++; //정체 상태 턴 수를 증가시킨다.
			else
				congest = 0;
			if (congest > 15) {//만약 정체 상태인 채로 15턴 이상 지났다면
				if (al_show_native_message_box(NULL, "Fanorona", "Draw", "Wanna end this game?", NULL, ALLEGRO_MESSAGEBOX_YES_NO) == 1) { //네이티브 다이얼로그를 이용해 플레이어(local)에게 무승부를 제안하라
				//제안 수락시 무승부 상태로 판을 업데이트
					for (i = 0; i < 45; i++) //check_board()에서 무승부를 판단하기 위한 조건
						board[i] = Empty;
				}
				else {
					//제안 거절시 정체 턴 수를 처음부터 헤아림
					congest = 0;
				}
			}
			state = check_board(board); //보드판 상태를 체크하고 state 변수에 저장한다.
			now_turn *= -1; //턴을 바꾼다. (말의 색상을 바꾼다.)
		}
		break;
	case Two_player: //오프라인 2인 모드
		init_guest(); // 전역 구조체로 선언되어있는 USER guest를 기본값으로 초기화
		p2 = &guest;  // 본 게임에서 상대(두번째) 플레이어를 설정
		//게임 진행
		while (state == No_win) {
			captured_pawn = player_move(now_turn, (p1_color == now_turn) ? p1 : p2, board);
			if (captured_pawn == 0)
				congest++;
			else
				congest = 0;
			if (congest > 15) {
				if (al_show_native_message_box(NULL, "Fanorona", "Draw", "Wanna end this game?", NULL, ALLEGRO_MESSAGEBOX_YES_NO) == 1) { //네이티브 다이얼로그를 이용해 플레이어(local)에게 무승부를 제안하라
				//제안 수락시 무승부 상태로 판을 업데이트
					for (i = 0; i < 45; i++) //check_board()에서 무승부를 판단하기 위한 조건
						board[i] = Empty;    //보드판의 모든 칸이 빈칸이어야 한다.
				}
				else {
					//제안 거절시 정체 턴 수를 처음부터 계산한다.
					congest = 0;
				}
			}
			state = check_board(board); //보드판 상태를 체크하고 state 변수에 저장한다.
			now_turn *= -1; //턴을 바꾼다. (말의 색상을 바꾼다.)
		}
		break;
	default:
		printf("entered wrong mode value\n"); //모드 입력 실패
		exit(0);
	}
	//게임이 끝났을 때
	switch (state) { //위의 루프에서 check_board()의 반환값 상태로 판단
	case Black_win://만약 흑색이 이겼다면
		al_show_native_message_box(NULL, "Fanorona", "Black win", "Black win!", NULL, ALLEGRO_MESSAGEBOX_QUESTION);//네이티브 다이얼로그로 흑색이 이김을 알림
		p1_color == White ? p1->lose++ : p1->win++; //p1은 언제나 local_user, local_user의 전적(승리) 업데이트
		break;
	case White_win://백색이 이겼다면
		al_show_native_message_box(NULL, "Fanorona", "White win", "White win!", NULL, ALLEGRO_MESSAGEBOX_QUESTION);//네이티브 다이얼로그로 백색이 이김을 알림
		p1_color == White ? p1->win++ : p1->lose++; //local_user의 전적(패배) 업데이트
		break;
	case Draw://무승부라면
		al_show_native_message_box(NULL, "Fanorona", "Draw", "It's a tie!", NULL, ALLEGRO_MESSAGEBOX_QUESTION); //네이티브 다이얼로그로 무승부가 됨을 알림
		p1->tie++; //local_user의 전적(무승부) 업데이트
		break;
	default:
		break;
	}
	if (login)
		update_user_result(); //db에 로그인된 유저의 전적을 갱신한다.
}

int set_cpu_level(void) {
	ALLEGRO_FONT *font = al_load_ttf_font("data/font/Menu.ttf", 35, 0);
	int redraw = false, cursor = 0;
	//화면 갱신 루틴을 사용한다.
	while (true) {
		al_wait_for_event(queue, &event);
		switch (event.type) {
		case ALLEGRO_EVENT_TIMER:
			redraw = true;
			break;
		case ALLEGRO_EVENT_KEY_DOWN:
			if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) { //왼쪽 화살표가 눌렸다면
				if (cursor > 0) //커서 유효성 검사 후 좌측으로 이동
					cursor--; 
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) { //오른쪽 화살표가 눌렸다면
				if (cursor < 2) //커서 유효성 검사 후 우측으로 이동
					cursor++;
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) { //엔터 키가 입력되었다면
				init_cpu(cursor); //선택한 값에 따라 cpu 구조체를 초기화
				al_destroy_font(font); //폰트를 메모리에서 해제하고
				return cursor; //함수를 끝낸다.
			}
			break;
		}
		if (redraw) { //화면 갱신 루틴
			al_draw_filled_rectangle(220, BOARD_Y + 150, 640, BOARD_Y + 350, RGB_KHAKI); //배경 사각형 그리기
			al_draw_multiline_text(font, RGB_BLACK, 285, 490, 360, 30, 0, "Which ENEMY do you wanna beat?");
			al_draw_text(font, (cursor == 0) ? RGB_NAVY : RGB_WHITE, 310, 590, ALLEGRO_ALIGN_CENTER, "LV0"); //ALLEGRO_ALIGN_CENTER 플래그 : 텍스트 중앙정렬
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
	//화면을 계속 갱신해야 하므로 반복문이 필요함
	while (true) {
		al_wait_for_event(queue, &event); //이벤트 큐(전역변수)에서 현재 발생한 이벤트를 받는다.
		switch (event.type) { //받은 이벤트가
		case ALLEGRO_EVENT_TIMER: //타이머에서 발생한 이벤트라면 (1/30초가 지났다면)
			redraw = true; //화면을 갱신하라(그리는 루틴은 아래로)
			break;
		case ALLEGRO_EVENT_KEY_DOWN://키보드에서 발생한 이벤트라면
			//어떤 키가 눌렸는지 판별하라
			if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) { //왼쪽 화살표가 눌렸다면
				cursor = White; //플레이어의 현재 선택은 흰색
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) { //오른쪽 화살표가 눌렸다면
				cursor = Black; //플레이어의 현재 선택은 검정색
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) { //엔터 키가 입력되었다면
				al_destroy_font(font); //함수를 끝낼 준비 (불러온 폰트를 메모리에서 해제)
				return cursor; //플레이어의 선택을 반환하고 함수를 종료
			}
			break;
		}
		if (redraw) { //만약 화면 갱신을 해야 한다면
			al_draw_filled_rectangle(220, BOARD_Y + 150, 640, BOARD_Y + 350, RGB_KHAKI); //황색으로 채워진 사각형을 주어진 좌표값에 맞게 디스플레이 버퍼에 그리라
			al_draw_multiline_text(font, RGB_BLACK, 285, 490, 300, 30, 0, "Who will be your loyal soldiers, your majesty?"); //디스플레이 버퍼에 텍스트를 그리라
			al_draw_text(font, (cursor == White) ? RGB_NAVY : RGB_WHITE, 290, 590, 0, "WHITE"); //플레이어의 선택에 따라 색상이 다르게 "WHITE" 문자를 그리라 (선택 시 남색, 선택이 안되었을시 백색)
			al_draw_text(font, (cursor == Black) ? RGB_NAVY : RGB_WHITE, 467, 590, 0, "BLACK"); //플레이어의 선택에 따라 색상이 다르게 "BLACK" 문자를 그리라 (선택 시 남색, 선택이 안되었을시 백색)
			al_flip_display();//디스플레이를 갱신하라 (디스플레이 버퍼에 있는 내용을 실제 디스플레이에 표시)
			redraw = false; //화면 갱신 변수를 다시 거짓으로 초기화. 다음 타이머 이벤트 발생 시 (1/30초가 지났을 시) 
		}
	}
}

int player_move(int color, USER *player, BOARD board[]) { //반환값은 삭제한 말 갯수
	int i = 0, j = 0, k = 0;
	int available_map[22] = { 0 }, trail[22] = { EOA, 0 };
	int new_pos = 0, cur_pos = 0;
	DIRECTION dir_map[8] = { North, Northwest, West, Southwest, South,  Southeast,  East, Northeast };
	int  captured = 0, move_type = 0;
	bool sequence = false;
	DIRECTION dir = 0;

	//상단 화면을 플레이어가 턴을 두고 있는 상태로 그리고, 화면 상단에 현재 어떤 플레이어가 진행하고 있는지 정보를 작성한 후 디스플레이 표시
	draw_scene(player, 1);
	show_turn_info(player, color);

	//턴이 시작할 때 전체 보드판에서 캡쳐링 무브로 이동 가능한 돌 검사
	for (i = 0; i < 45; i++) {
		if (board[i] == color) {
			for (j = 0; j < 8; j += (i % 2 + 1)) {//j+=(i%2+1) ? 짝수칸은 8방향으로 이동 가능(i++), 홀수 칸은 4방향으로만 이동 가능 (i+=2)
				if (is_moveable(i, i + dir_map[j], dir_map[j], board) && //dir_map[j] 방향으로 이동할 수 있는가?
					(is_catchable(i, i + dir_map[j] * 2, dir_map[j], board) || is_catchable(i, i + dir_map[j] * -1, dir_map[j], board)) //이동할 위치에서 (일직선 상에) 포워드 혹은 백으로 지울 수 있는 말이 있는가?
					) {
					available_map[k++] = i; //플레이어가 이동 가능한 말의 배열에 현재 좌표(에 있는 말)를 추가하라
					break;
				}
			}
		}
	}
	available_map[k] = EOA; //유효한 데이터의 입력이 끝났음을 알리도록 EOA(End of Array)를 추가한다.

	//캡쳐링 무브와 파이카 무브를 구분해서 시행
	if (available_map[0] != EOA) { //만약 캡처링 무브가 가능한 말이 하나 이상 있다면 (배열이 비어있지 않았다면)
		cur_pos = get_selection(available_map, 22, board); //get_selection으로 available_map에 있는 좌표 중 하나를 플레이어 선택으로 받아 cur_pos에 저장한다.
		do {
			//선택한 말에 대한 캡쳐링 무브 시행 (이동 후에도 삭제할 수 있는 말이 계속 남아있는 한 반복한다.)

			//선택한 말을 어디로 이동할지 결정 (빈 칸 검사, 중복 이동 검사, 삭제 가능 검사)
			for (i = 0, j = 0; i < 8; i += cur_pos % 2 + 1) {
				if (is_moveable(cur_pos, cur_pos + dir_map[i], dir_map[i], board) && //dir_map[i]의 방향으로 이동이 가능한가? (빈 칸인가?)
					is_first_time(cur_pos + dir_map[i], trail) && //이동 할 빈칸을 혹시 이전에 간 적이 있는가
					(is_catchable(cur_pos, cur_pos + dir_map[i] * 2, dir_map[i], board) == true || //그 칸으로 이동했을 때 포워드로 지울 수 있는 말이 있는가?
						is_catchable(cur_pos, cur_pos + dir_map[i] * -1, dir_map[i], board) == true))  //혹은 백으로 지울 수 있는 말이 있는가?
					available_map[j++] = cur_pos + dir_map[i]; //세가지 조건에 맞는다면 현재 좌표를 선택지에 저장하라
			}
			if (trail[0] != EOA) {
				available_map[j++] = cur_pos;
			}
			available_map[j] = EOA; //유효한 데이터의 끝을 알림

			//플레이어의 선택을 받음 & 보드에서 실제로 이동 & 선택을 바탕으로 어떤 방향으로 이동했는지 확인하기
			new_pos = get_selection(available_map, cur_pos, board);
			play_piecedrop(); //말을 이동하는 소리를 재생
			board[cur_pos] = Empty;
			board[new_pos] = color;
			dir = new_pos - cur_pos;

			if (new_pos == cur_pos)
				return captured;

			//이동 직후에 삭제를 시작
			//이동한 위치에서 포워드 혹은 백 공격이 가능한지 확인한다. (방향으로 이동할 수 있는 제일 가까운 칸만 확인한다.)
			move_type = 0; //포워드/백 여부를 나타내는 변수를 0으로 초기화
			if (is_catchable(new_pos, new_pos + dir, dir, board)) //포워드 위치에 적이 있음
				move_type += 1; //move_type = 1
			if (is_catchable(new_pos, new_pos + dir * -2, dir, board)) //백 위치에 적이 있음
				move_type -= 1; //move_type = -1

			if (move_type == 0) { //포워드/백이 둘 다 가능할 시(여전히 move_type이 0일 시에) 플레이어의 선택을 받는다.
				//포워드와 백이 되는 위치를 배열에 추가해서 플레이어의 선택을 받도록 get_selection으로 넘김
				available_map[0] = new_pos + dir;
				available_map[1] = new_pos + dir * -2;
				available_map[2] = EOA;
				if (get_selection(available_map, new_pos, board) == new_pos + dir) //get_selection에서 받은 값이 포워드 위치와 같다면
					move_type = 1; //포워드를 수행하도록 변수 변경
				else
					move_type = -1; //백을 수행하도록 변수 변경
			}
			if (move_type == 1) {//포워드를 수행할 수 있을때
				for (i = 1; board[new_pos + dir * i] == color * -1; i++) {  //일직선에 있는 이어진 상대편 말 전체를 대상으로(dir * i) 삭제를 수행한다, 만약 중간에 Empty나 자신의 말이 있다면 반복은 종료된다.
					if (is_catchable(new_pos, new_pos + dir * i, dir, board)) //만약 삭제할 수 있는 말이라면
					{
						board[new_pos + dir * i] = Empty; //그 위치는 빈칸으로 전환한다
						animate_capturing(color*-1, new_pos + dir * i, board); //그 말에 대해 삭제되는 애니메이션을 수행한다
						captured++;
					}
				}
			}
			else if (move_type == -1) { //백을 수행할 수 있을 때
				for (i = 2; board[new_pos + dir * -i] == color * -1; i++) { //일직선에 있는 이어진 상대편 말 전체를 대상으로(dir * i) 삭제를 수행한다.
					if (is_catchable(new_pos, new_pos + dir * -i, dir, board)) //포워드와 같음
					{
						board[new_pos + dir * -i] = Empty;
						animate_capturing(color*-1, new_pos + dir * -i, board);
						captured++;
					}
				}
			}

			for (i = 0; trail[i] != EOA; i++);//배열의 끝으로 i를 증가시킨다.
			trail[i] = cur_pos; //배열의 마지막 요소에 현재 위치를 추가한다.
			trail[i + 1] = EOA; //다시 EOA를 추가한다.

			sequence = false;
			for (i = 0; i < 8; i += (new_pos % 2 + 1)) {
				if (is_moveable(new_pos, new_pos + dir_map[i], dir_map[i], board) && //현재 위치(이동한 위치)에서 이동 가능하고 지나온 위치가 아니며
					is_first_time(new_pos + dir_map[i], trail) && //포워드 혹은 백으로 삭제할 수 있는 말이 하나 이상 있는가
					(is_catchable(new_pos, new_pos + dir_map[i] * 2, dir_map[i], board) == true || is_catchable(new_pos, new_pos + dir_map[i] * -1, dir_map[i], board) == true))
				{
					sequence = true; //연속으로 캡쳐링 무브를 수행할 수 있도록 한다.
					cur_pos = new_pos; //말의 '현재 위치'를 갱신한다.
					break; //하나라도 있으면 되므로 나머지 칸은 검사할 필요 없음, 반복문을 빠져나감
				}
			}
		} while (sequence); //sequence가 true이면 현재 말에 대해서 캡쳐링 무브를 계속 진행한다.
	}
	else {	//상대편 말을 삭제할 수 있는 말이 하나도 없다면
	//보드를 새로 검사해서 파이카 무브를 진행한다. (돌을 지우지 않는 이동, 캡처링 무브가 이동할 수 없을 때만 사용 가능하다.)
		for (i = 0; i < 45; i++) {
			if (board[i] == color) {
				for (j = 0; j < 8; j += (i % 2 + 1)) {
					if (is_moveable(i, i + dir_map[j], dir_map[j], board)) { //이동 가능한 빈칸이 있는 지만 검사
						available_map[k++] = i; //이동 가능한 말의 배열에 현재 좌표를 추가해라
						break;
					}
				}
			}
		}
		available_map[k] = EOA; //유효한 데이터 입력을 종료

		cur_pos = get_selection(available_map, 22, board); //사용자의 선택을 받는다.
		for (i = 0, j = 0; i < 8; i += cur_pos % 2 + 1) {//선택한 말에 대해서만 다시 이동 가능한 빈칸을 검사
			if (is_moveable(cur_pos, cur_pos + dir_map[i], dir_map[i], board)) { //조건은 위와 같음
				available_map[j++] = cur_pos + dir_map[i];
			}
		}
		available_map[j] = EOA;
		new_pos = get_selection(available_map, cur_pos, board); //이동할 위치를 저장
		play_piecedrop(); //이동 사운드 재생
		board[cur_pos] = Empty; //실제로 보드판을 변경
		board[new_pos] = color;
	}
	draw_scene(player, 2); //상단 화면에 플레이어가 수를 두는 모습을 그린다. (0.5초동안 보여줌)
	return captured;//현재까지 삭제한 말을 반환하고 player_move()를 종료한다.
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

	//상단 화면을 cpu가 턴을 두고 있는 상태로 그리고, 화면 상단에 현재 어떤 플레이어가 진행하고 있는지 정보를 작성한 후 디스플레이 표시
	draw_scene(cpu, 1);
	show_turn_info(cpu, color);

	//player_move와 기본 수행 방식은 같다.

	//턴이 시작할 때 전체 보드판에서 캡쳐링 무브로 이동 가능한 돌 검사
	for (i = 0; i < 45; i++) {
		if (board[i] == color) {
			for (j = 0; j < 8; j += (i % 2 + 1)) {//j+=(i%2+1) ? 짝수칸은 8방향으로 이동 가능(i++), 홀수 칸은 4방향으로만 이동 가능 (i+=2)
				if (is_moveable(i, i + dir_map[j], dir_map[j], board) && //dir_map[j] 방향으로 이동할 수 있는가?
					(is_catchable(i, i + dir_map[j] * 2, dir_map[j], board) || is_catchable(i, i + dir_map[j] * -1, dir_map[j], board)) //이동할 위치에서 (일직선 상에) 포워드 혹은 백으로 지울 수 있는 말이 있는가?
					) {
					available_map[k++] = i; //플레이어가 이동 가능한 말의 배열에 현재 좌표(에 있는 말)를 추가하라
					break;
				}
			}
		}
	}
	available_map[k] = EOA; //유효한 데이터의 입력이 끝났음을 알리도록 EOA(End of Array)를 추가한다.

	//캡쳐링 무브와 파이카 무브를 구분해서 시행
	if (available_map[0] != EOA) { //만약 캡처링 무브가 가능한 말이 하나 이상 있다면 (배열이 비어있지 않았다면)
		
		//이동시킬 말을 선택한다.
		//이 switch문은 player_move의 get_selection()을 대체한다.
		switch (level) {
		case 0:	case 1: default: //레벨 0, 1의 AI에서는 선택할 말을 랜덤으로 선택한다.
			cur_pos = random_select(available_map);
			break;
		case 2: //레벨 2의 ai는 calculate_pawn을 이용해서 캡처링 무브가 가능한 모든 말을 계산하고 그 중 최고 점수를 얻을 수 있는 하나를 선택한다.
			for (i = 0; available_map[i] != EOA; i++) { //이동 가능한 전체 말에 대해서 계산
#ifdef DEBUG 
				printf("\n\n검사할 말 : [%d]\n", available_map[i]);
#endif
				score_map[i] = calculate_pawn(available_map[i], color, 0, temp_trail, board, temp_move); //각 말의 최대 점수를 score_map에 저장한다.
				temp_trail[0] = EOA; //temp_trail은 calculate_pawn 계산용 이동 역사 저장 배열이다.
			}
			score_map[i] = EOA;
			for (i = 0; score_map[i] != EOA; i++) { 
				if (score_map[i] > max_score) { //최대 점수를 찾는다.
					max_score = score_map[i]; //최대 점수를 갱신한다.
					cur_pos = available_map[i]; //현재 위치도 그 말로 갱신한다.
				}
			}

#ifdef DEBUG 
			printf("\n--------------movement 갱신용 검사------------------\n");
#endif
			calculate_pawn(cur_pos, color, 0, temp_trail, board, move_map); //선택한 말에 대해 다시 한번 calculate_pawn()을 수행하여 이동 경로를 move_map에 저장한다.
#ifdef DEBUG
			printf("\n-----------------선택한 말 : %d (점수 : %d)--------\n", cur_pos, max_score);
			printf("선택한 말의 이동 경로: ");
			for (i = 0; move_map[i] != 0; i++)
				printf("%2d ", move_map[i]);
#endif
			break;
		}

		//player_move()와 같음
		al_rest(0.5);
		do {
			//선택한 말을 어디로 이동할지 결정 (빈 칸 검사, 중복 이동 검사, 삭제 가능 검사)
			for (i = 0, j = 0; i < 8; i += cur_pos % 2 + 1) {
				if (is_moveable(cur_pos, cur_pos + dir_map[i], dir_map[i], board) && //dir_map[i]의 방향으로 이동이 가능한가? (빈 칸인가?)
					is_first_time(cur_pos + dir_map[i], trail) &&
					(is_catchable(cur_pos, cur_pos + dir_map[i] * 2, dir_map[i], board) == true || //그 칸으로 이동했을 때 포워드로 지울 수 있는 말이 있는가?
						is_catchable(cur_pos, cur_pos + dir_map[i] * -1, dir_map[i], board) == true))  //혹은 백으로 지울 수 있는 말이 있는가?
					available_map[j++] = cur_pos + dir_map[i]; //세가지 조건에 맞는다면 현재 좌표를 선택지에 저장하라
			}
			if (trail[0] != EOA) {
				available_map[j++] = cur_pos;
			}
			available_map[j] = EOA;

			//현재 말을 어떤 위치로 이동시킬지 결정한다.
			//이 switch문은 player_move의 get_selection()을 대체한다.
			switch (level) {
			case 0: //레벨 0 AI는 이동할 위치도 랜덤으로 선택한다.
				new_pos = random_select(available_map);
				break;
			case 1: //레벨 1 AI는 랜덤으로 선택한 말에 대해서 '현재 이동에 대해서(연속적인 이동은 고려하지 않고)' 가장 점수를 많이 얻을 수 있는 위치를 선택한다.
				for (i = 0; available_map[i] != EOA; i++)
					score_map[i] = calculate_pos(cur_pos, color, available_map[i] - cur_pos, board); //이동 가능한 각 자리의 점수를 계산
				for (i = 0, max_score = 0; available_map[i] != EOA; i++)
					if (abs(score_map[i]) >= abs(max_score)) { //최댓값을 찾는 과정, 레벨 1에서 반환하는 점수는 백과 포워드를 구분하기 위해서 음수와 양수가 있기 때문에 절대값으로 계산해야 한다.
						max_score = score_map[i];
						new_pos = available_map[i]; //상대편 말을 최대로 지울 수 있는 위치를 이동 위치(NEW_POS)로 선택한다.
					}
				break;
			case 2: default:
				new_pos = abs(move_map[cnt++]); //함수의 제일 처음에 계산해두었던 이동 경로를 선택한다. 백과 포워드를 구분하기 위해 좌표에다 부호를 부여했었으므로 절댓값을 사용해야 한다.
				break;
			}

			play_piecedrop(); //말을 이동하는 효과음을 재생한다.
			board[cur_pos] = Empty;
			board[new_pos] = color;
			dir = new_pos - cur_pos;

			if (new_pos == cur_pos)
				return captured;

			//이동 직후에 삭제를 시작
			//이동한 위치에서 포워드 혹은 백 공격이 가능한지 확인한다. (방향으로 이동할 수 있는 제일 가까운 칸만 확인한다.)
			move_type = 0; //포워드/백 여부를 나타내는 변수를 0으로 초기화
			if (is_catchable(new_pos, new_pos + dir, dir, board)) //포워드 위치에 적이 있음
				move_type += 1; //move_type = 1
			if (is_catchable(new_pos, new_pos + dir * -2, dir, board)) //백 위치에 적이 있음
				move_type -= 1; //move_type = -1

			if (move_type == 0) { //포워드/백이 둘 다 가능할 시(여전히 move_type이 0일 시에) 각 AI별 처리를 수행한다.
				//이 switch문은 get_selection()을 대체한다.
				switch (level) {
				case 0: default:
					//레벨 0 AI는 포워드와 백 중 하나를 랜덤으로 선택한다.
					available_map[0] = new_pos + dir;
					available_map[1] = new_pos + dir * -2;
					available_map[2] = EOA;
					if (random_select(available_map) == new_pos + dir) //get_selection에서 받은 값이 포워드 위치와 같다면
						move_type = 1; //포워드를 수행하도록 변수를 변경한다.
					else
						move_type = -1; //백을 수행하도록 변수 변경
				case 1:
					//레벨 1 AI는 아까 계산했던 결과의 부호에 따라서 백과 포워드를 선택한다.
					if (max_score > 0) //계산 결과가 양수면 포워드 진행 아니면 백 진행
						move_type = 1;
					else
						move_type = -1;
					break;
				case 2:
					//레벨 2 AI는 이동 경로의 좌표가 양수이면 포워드로, 음수이면 백으로 진행한다.
					if (move_map[cnt] > 0)
						move_type = 1;
					else
						move_type = -1;
					break;
				}
				al_rest(0.5);
			}
			//지금부터 캡처링 무브의 끝까지는 player_move()의 계산과 동일하다.
			if (move_type == 1) {//포워드를 수행할 수 있을때
				for (i = 1; board[new_pos + dir * i] == color * -1; i++) {  //일직선에 있는 이어진 상대편 말 전체를 대상으로(dir * i) 삭제를 수행한다, 만약 중간에 Empty나 자신의 말이 있다면 반복은 종료된다.
					if (is_catchable(new_pos, new_pos + dir * i, dir, board)) //만약 삭제할 수 있는 말이라면
					{
						board[new_pos + dir * i] = Empty; //그 위치는 빈칸으로 전환한다
						animate_capturing(color*-1, new_pos + dir * i, board); //그 말에 대해 삭제되는 애니메이션을 수행한다
						captured++;
					}
				}
			}
			else if (move_type == -1) { //백을 수행할 수 있을 때
				for (i = 2; board[new_pos + dir * -i] == color * -1; i++) { //일직선에 있는 이어진 상대편 말 전체를 대상으로(dir * i) 삭제를 수행한다.
					if (is_catchable(new_pos, new_pos + dir * -i, dir, board)) //포워드와 같음
					{
						board[new_pos + dir * -i] = Empty;
						animate_capturing(color*-1, new_pos + dir * -i, board);
						captured++;
					}
				}
			}
			//캡쳐링 무브를 연속해서 수행할 때 본 위치로 못 돌아오도록 이동을 시행한 칸의 좌표를 좌표 역사에 추가해서 막음
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
	else {	 //파이카 무브
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

		//이동을 수행할 말을 선택한다.
		//이 switch문은 player_move()의 get_selection()을 대체한다.
		switch (level) {
		case 0: case 1: //레벨 0, 레벨 1 AI는 이동할 말 / 이동 위치를 랜덤으로 선택한다.
			cur_pos = random_select(available_map); //컴퓨터의 선택을 받는다.
			for (i = 0, j = 0; i < 8; i += cur_pos % 2 + 1) {//선택한 말에 대해서만 다시 이동 가능한 빈칸을 검사
				if (is_moveable(cur_pos, cur_pos + dir_map[i], dir_map[i], board)) { //조건은 위와 같음
					available_map[j++] = cur_pos + dir_map[i];
				}
			}
			available_map[j] = EOA;
			new_pos = random_select(available_map);
			break;
		case 2: default: //레벨 2 AI는 이동할 말을 무작위로 선택하고, calculate_pawn()을 이용해 파이카 무브 시 랜덤하게 선택된 말이 잡히지 않도록 도망다닌다.

			//calculate_pawn() 함수에서는 파이카 무브에서 어떤 경로를 가도 상대편 말에 잡힌다면 이동경로를 설정하지 않고 그냥 함수를 종료한다.
			//계속 여러 말을 찾아서 이동 가능한 말이 있는지 반복하도록 하였다.
			while (move_map[0] == EOA) {
				cur_pos = random_select(available_map); //이동할 말을 무작위로 선택한다.
				calculate_pawn(cur_pos, color, 0, trail, board, move_map); //이동할 위치를 검사한다.
				new_pos = move_map[0]; //이동경로는 move_map[0] 하나밖에 저장되지 않는다.

				//파이카 무브를 여러 번 시행해도 상대 말에게 잡힐 때 (어떤 위치로 이동해도 희생이 불가피할 때)
				//레벨 0, 1 AI처럼 위에서 cur_pos에 저장된 말을 랜덤하게 움직인다.
				if (cnt++ > 5) { //캡쳐링에서는 cnt가 move_map을 선택하는 인덱스로 사용, 파이카 무브에서는 cnt가 calculate_pawn() 횟수를 세는 변수로 사용된다.
					for (i = 0, j = 0; i < 8; i += cur_pos % 2 + 1) { //선택된 말에 대해서만 다시 이동 가능한 빈칸을 검사한다.
						if (is_moveable(cur_pos, cur_pos + dir_map[i], dir_map[i], board)) { //조건은 위와 같음
							available_map[j++] = cur_pos + dir_map[i]; //이동 가능한 칸을 available_map에 저장한다.
						}
					}
					available_map[j] = EOA;
					new_pos = random_select(available_map); //이동할 위치 역시 랜덤하게 선택한다.
					break;
				}

			}
			break;
		}
		al_rest(0.5);
		//이동할 위치를 저장
		play_piecedrop(); //이동 사운드 재생
		board[cur_pos] = Empty; //실제로 보드판을 변경
		board[new_pos] = color;
	}
	draw_scene(cpu, 2); //상단 화면에 플레이어가 수를 두는 모습을 그린다. (0.5초동안 보여줌)
	return captured;//현재까지 삭제한 말을 반환하고 computer_move()를 종료한다.
}

int calculate_pawn(int pos, int color, int depth, int *trail, BOARD ori_board[], int *movement) {
	//선택 받은 말에서 최선의 경로를 찾아내는 함수. 재귀 호출을 이용한다.
	int available_map[9] = { 0 }, score_map[9] = { 0 }, branch_map[9][22] = { 0 };
	int i = 0, j = 0, k = 0, score = 0, move_type = 0, new_pos = 0, max_score = 0;
	bool sequence = 0;
	DIRECTION dir = 0, dir_map[8] = { North, Northwest, West, Southwest, South,  Southeast,  East, Northeast };
	BOARD new_board[45] = { 0 };

	//1. 캡처링 무브를 시행할 수 있는 방향이 있는지 탐색을 시행한다.
	for (i = 0; i < 8; i += pos % 2 + 1) {
		if (is_moveable(pos, pos + dir_map[i], dir_map[i], ori_board) && //dir_map[i]의 방향으로 이동이 가능한가? (빈 칸인가?)
			is_first_time(pos + dir_map[i], trail) &&
			(is_catchable(pos, pos + dir_map[i] * 2, dir_map[i], ori_board) == true ||
				is_catchable(pos, pos + dir_map[i] * -1, dir_map[i], ori_board) == true))
			available_map[j++] = pos + dir_map[i];
	}
	available_map[j] = EOA;
#ifdef DEBUG
	printf("현재위치 (%d) avail_map 리스트 :\n", pos);
	for (j = 0; available_map[j] != EOA; j++)
		printf("avail_map[%d] : %d ", j, available_map[j]);
	printf("\n");
#endif
	//2. 만약 캡처링 무브를 시행할 수 있는 위치가 없다면 파이카 무브를 시행한다.
	//본 함수에서의 파이카 무브는 선택한 말이 이동했을 시 상대편의 말이 자신을 잡지 못하도록 잡히지 않는 칸을 찾아 이동한다.
	if (available_map[0] == EOA) { 
		//호출 시에 받은 원본 보드판 값을 변경하지 않도록 새 보드판에 내용을 복사한다.
		for (j = 0; j < 45; j++) { 
			new_board[j] = ori_board[j];
		}

		//선택한 말의 파이카 무브용 8방향 검사. (주위의 이동할 수 있는 빈 칸만을 검사한다.)
		for (i = 0, j = 0; i < 8; i += pos % 2 + 1) { 
			if (is_moveable(pos, pos + dir_map[i], dir_map[i], new_board))
				available_map[j++] = pos + dir_map[i]; //이동할 수 있는 칸을 available_map에 추가한다.
		}
		available_map[j] = EOA;

		//위에서 추가한 빈 칸에 대해 검사를 수행한다.
		for (i = 0; available_map[i] != EOA; i++) { 
			//new_board의 값을 변경해 실제로 이동한 것으로 가정한다.
			new_board[pos] = Empty;
			new_board[available_map[i]] = color;

			sequence = true; //sequence는 파이카 무브 검사에서 위치의 유효성(이곳으로 이동할 수 있는지)을 검사하는 변수로 사용된다.

			//'이동한 위치'에서 주위 8방향을 검사하고, 8방향보다 한칸 더 먼 위치를 검사한다. 8방향 x 2칸은 상대편 말이 자신의 말을 포워드/백으로 잡을 수 있는 거리이다.
			

			for (j = 0; j < 8 && sequence == true; j += available_map[i] % 2 + 1) { //주위의 8방향에 대해 검사한다.
				//현재 말이 이동한 위치에서 백으로 잡힐 수 있는지 검사 (1칸 위 -- 상대가 백으로 지울 수 있는 위치)
				if (new_board[available_map[i] + dir_map[j]] == color * -1) { //만약 백으로 잡힐 수 있는 인접한 방향에 상대편 말이 있다면,
					for (k = 0; k < 8 && sequence == true; k += available_map[i] + dir_map[j] % 2 + 1) { //상대편 말(available_map[i] + dir_map[j])의 입장에서
						if (is_catchable(available_map[i] + dir_map[j], available_map[i], dir_map[k], new_board)) { //8방향중 어떠한 방향으로 잡힐 수 있다면 (true)
							sequence = false; //이 위치는 선택하지 않도록 하라.
							break;
						}
					}
				}
				//현재 말이 이동한 위치에서 포워드로 잡힐 수 있는지 검사 (2칸 위 -- 상대가 포워드로 말을 잡을 수 있는 위치)
				if (new_board[available_map[i] + dir_map[j] * 2] == color * -1) { //만약 포워드로 잡힐 수 있는 인접한 방향에 상대편 말이 있다면,
					for (int k = 0; k < 8 && sequence == true; k += (available_map[i] + dir_map[j] * 2) % 2 + 1) { //상대편 말(available_map[i] + dir_map[j] * 2)의 입장에서
						if (is_catchable(available_map[i] + dir_map[j] * 2, available_map[i], dir_map[k], new_board)) { //8방향중 어떠한 방향으로 잡힐 수 있다면 (true)
							sequence = false; //이 위치는 선택하지 않도록 하라.
							break;
						}
					}
				}
			}
			if (sequence == true) { //만약 이동해도 안전한 위치를 찾았다면
				movement[0] = available_map[i]; //그 위치를 호출 당시의 이동 경로에 설정하고
				return 0; //함수를 종료하라.
			}
			//다음 방향을 계산하기 위해서 보드를 원상복귀시킨다.
			new_board[pos] = color;
			new_board[available_map[i]] = Empty;
		}
		return 0; //어느 방향으로도 잡힐 수 있을 때는 다른 말을 검사하도록 아무 일도 하지 않고 함수를 종료한다.
	}
	//2. 만약 캡쳐링 무브를 진행 할 수 있는 위치가 있다면 캡처링 무브에 대한 검사를 수행한다.
	else {
		for (i = 0; available_map[i] != EOA; i++) { //available_map은 선택한 말에 대해 캡처링 무브로 이동할 수 있는 방향(위치)정보를 가지고 있다.
			//상위 호출에서 받은 보드판 값을 변경하지 않도록 새 보드판에 내용을 복사한다.
			for (j = 0; j < 45; j++) { 
				new_board[j] = ori_board[j];
			}
			new_pos = available_map[i];
			dir = new_pos - pos;

			//3. 여기서부터 player_move()와 동일한 수행
			//백포워드 결정
			if (calculate_pos(pos, color, dir, new_board) > 0)
				move_type = 1;
			else
				move_type = -1;

			//선택된 말의 위치 변경
			new_board[pos] = Empty;
			new_board[new_pos] = color;

			if (move_type == 1) { //포워드를 수행할 수 있을때
				for (j = 1; new_board[new_pos + dir * j] == color * -1; j++) {
					if (is_catchable(new_pos, new_pos + dir * j, dir, new_board)) {
						new_board[new_pos + dir * j] = Empty; //상대편 말의 위치는 빈칸으로 전환하고
						score++; //현재 시행의 점수를 증가시킨다.
					}
				}
			}
			else { //백을 수행할 수 있을 때
				for (j = 2; new_board[new_pos + dir * -j] == color * -1; j++) {
					if (is_catchable(new_pos, new_pos + dir * -j, dir, new_board)) {
						new_board[new_pos + dir * -j] = Empty; //상대편 말의 위치는 빈칸으로 전환하고
						score++; //현재 시행의 점수를 증가시킨다.
					}
				}
			}

			//캡쳐링 무브를 연속해서 수행할 때 본 위치로 못 돌아오도록 이동을 시행한 칸의 좌표를 좌표 역사에 추가해서 막음
			for (j = 0; trail[j] != EOA; j++);
			trail[j] = pos;
			trail[j + 1] = EOA;

			//4. 재귀 호출 결정
			//다음 depth로 진행이 가능한가? --> 캡쳐링 무브를 한번 더 시행할 수 있는가?
			sequence = false; //bool sequence - 재귀 호출 여부를 결정한다.
			for (j = 0; j < 8; j += (new_pos % 2 + 1)) { //이동한 위치에서 다시 캡쳐링  무브에 대한 검사를 시행
				if (is_moveable(new_pos, new_pos + dir_map[j], dir_map[j], new_board) &&
					is_first_time(new_pos + dir_map[j], trail) &&
					(is_catchable(new_pos, new_pos + dir_map[j] * 2, dir_map[j], new_board) == true || is_catchable(new_pos, new_pos + dir_map[j] * -1, dir_map[j], new_board) == true))
				{
					sequence = true; //재귀 호출을 수행한다.
					break; //다른 경로는 더 검사할 필요가 없어 for문을 빠져나간다.
				}
			}
#ifdef DEBUG
			printf("%d번째 이동, 이동위치: avail_map[%d] (%d)  점수: %d\n", depth, i, new_pos, score);
#endif
			if (sequence) { //5. 진행 가능할 경우 재귀 호출 - 재귀 호출의 결과는 현재까지 지운 말의 갯수이다. 현재의 상황에서 이동할 수 있는 각 경로의 결과를 score_map에 저장한다.
				score_map[i] = calculate_pawn(new_pos, color, depth + 1, trail, new_board, movement); 

			//다음으로 진행하기 전에 방금 진행했던 경로를 branch_map에 저장한다. (movement 백업)
				for (j = 0; j < 22; j++) {
					branch_map[i][j] = movement[j];
				}
				branch_map[i][j] = EOA;
			}
			else { //5. 캡쳐링 무브를 더 이상 연속으로 진행할 수 없을 경우 (리프에 도달했을 경우)
				movement[depth] = (move_type == 1) ? new_pos : new_pos * -1; //진행 경로 배열의 현재 depth에 현재 위치를 저장한다. (이 경로에서 백으로 진행했으면 -1을 곱한다.)
				movement[depth + 1] = EOA; //배열 끝을 알리는 기호인 EOA를 저장
				score_map[i] = score; //재귀호출로 들어가야 할 자리에는 자신의 점수가 그대로 들어온다.
				score = 0; //재귀호출 대신 for문을 진행해야 하므로 (다른 자리를 검사해야 하므로) score(지운 말 갯수)를 0으로 초기화한다.
			}
		}
		score_map[i] = EOA; //score_map에 입력을 마치면 EOA를 저장한다.
		//6. 저장된 score_map을 탐색해 최대 점수를 가진 경로를 찾고 movement(진행 경로)에 이번 시행의 최대 위치와 함께 저장한다.
		for (i = 0, max_score = 0; score_map[i] != EOA; i++) {
			if (score_map[i] > max_score) { //만약 max_score가 갱신되었다면
				max_score = score_map[i];
				for (j = 0; j < 22; j++) { //최대 점수를 가지고 있는 경로의 데이터 불러오기 (score_map의 인덱스와 branch_map[i]의 인덱스는 같다.)
					movement[j] = branch_map[i][j]; //movement에 저장하면 상위 호출의 branch_map에 경로들이 다 저장된다.
				}
				movement[depth] = available_map[i] * move_type; //실제 시행 시 백과 포워드를 구분하기 위해서 move_type을 곱해서 저장한다. (+는 포워드 , -는 백)
			}
		}
#ifdef DEBUG
		printf("[선택 : %d]\n\n", movement[depth]);
#endif
		return score + max_score; //7. 다음으로 재귀호출했던 결과값과 자신의 점수를 합쳐서 상위 호출로 반환한다.
	}
}

int calculate_pos(int cur_pos, int color, DIRECTION dir, int board[]) {
	int result[2] = { 0 }, move_type = 0, i = 0;
	//이동을 진행할 말과 이동할 방향, 보드판을 입력받는다. 백 혹은 포워드로 최대 몇 개의 말을 지울 수 있는지 계산하여 점수를 반환한다.


	for (i = 2; board[cur_pos + dir * i] == color * -1; i++) { //포워드로 진행했을 때
		if (is_catchable(cur_pos, cur_pos + dir * i, dir, board)) //삭제할 수 있는 말이 있다면
		{
			result[0]++; //삭제 가능 갯수(포워드)를 늘린다.
		}
	}
	for (i = 1; board[cur_pos + dir * -i] == color * -1; i++) { //백으로 진행했을 때
		if (is_catchable(cur_pos, cur_pos + dir * -i, dir, board)) //삭제할 수 있는 말이 있다면
		{
			result[1]++; //삭제 가능 갯수(백)를 늘린다.
		}
	}


	if (result[1] > result[0]) //백으로 이동했을 시에
		return result[1] * -1; //점수에 -1을 곱해 출력한다, computer_move()로 돌아갔을 때 백, 포워드 결정에 사용된다.
	else
		return result[0]; //점수를 그대로 출력한다.
}

//이동하려는 칸이 보드 범위에서 유효한가? (같은 열이나 행으로 이동할 수 있는가?)
bool is_moveable(int src, int dest, DIRECTION dir, BOARD board[]) { //출발지점 좌표와 목적지 좌표, 검사할 방향, 검사할 보드를 인자로 받는다.
	//만약 현재 목적지 내부에 아무 말도 놓여있지 않으며 ,목적지의 좌표가 유효한 보드 범위에서 유효하다면(0~45)
	if (board[dest] == Empty && dest < 45 && dest >= 0) {
		//검사할 방향에 따라서 다른 동작을 수행한다.
		//이차원 배열과 달리 일차원 배열은 어디까지가 같은 행이고 열인지 그 자체로는 구분할 수 없다. 그래서 이차원으로 컨버팅하는 과정을 거쳐서 행열이 같은지 확인한다.
		switch (dir) {
		case North: case South: //검사할 방향이 북쪽 혹은 남쪽이라면 (src와 dest가 같은 열인지 확인한다. -- 즉 둘의 x좌표는 같아야 한다.)
			if (src % 9 == dest % 9) //일차원 값에서 mod 9 연산을 하면 0~8범위의 x좌표가 나온다.
				return true;
			break;
		case West: case East: //검사할 방향이 서쪽 혹은 동쪽이라면 (src와 dest가 같은 행인지 확인한다. -- 즉 둘의 y좌표는 같아야 한다.)
			if (src / 9 == dest / 9) //일차워 값에서 divide 9 연산을 하면 0~4 범위의 y좌표가 나온다.
				return true;
			break;
		case Northwest: case Southeast: //검사할 방향이 북서 혹은 남동이라면 (src, dest의 x좌표 차이와 y좌표 차이는 똑같이 나와야 한다.)
			if (src / 9 - dest / 9 == src % 9 - dest % 9) //(src와 dest의 y좌표, src와 dest의 x좌표는 똑같이 -1 차이난다.)
				return true;
			break;
		case Northeast: case Southwest: //검사할 방향이 남서 혹은 북동이라면 (src. dest의 x좌표 차이와 y좌표 차이는 음양이 반대로 나와야 한다.)
			if (src / 9 - dest / 9 == -(src % 9 - dest % 9))
				return true;
			break;
		default: //올바르지 않은 방향값에선 아무 연산도 수행하지 않는다.
			break;
		}
	}
	//true가 반환되지 않았다면 자동적으로 false를 반환한다.
	return false;
}

int random_select(int available[]) {
	int i = 0;
	for (i = 0; available[i] != EOA; i++);
	return available[rand() % i];
}

//현재 위치에서 dir방향에 있는 dest칸의 말을 잡을 수 있는가
bool is_catchable(int src, int dest, DIRECTION dir, BOARD board[]) {
	//목적지 칸의 내용이 상대편 말인가 && 목적지가 보드 범위 내인가
	if (board[dest] == board[src] * -1 && dest < 45 && dest >= 0) {
		switch (dir) { //위의 조건을 통과했다면 방향에 따라서
		case North: case South: //is_moveable과 같은 내용 (출발지와 목적지가 같은 행이나 같은 열 등에 있어서 유효한지)
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
		if (trail[i] == position) //position이 현재까지 지나온 좌표들 중 하나와 겹친다면
			return false;
	return true;
}

int campaign_chat(int lv) {
	static int num, score = 20, saved_ans[5]; //campaign_chat()이 몇 번 호출되었는지와 현재 사용자의 점수, 각 호출 시 마다 어떤 대답을 선택했는지 저장한다.
	int i = 0, row = 0;
	bool answered = false;
	DIALOG dialog[5] = { 0 }; //대화 횟수는 최대 5회를 넘지 않는다.
	DIALOG_STATE state = In_progress; //현재 다이얼로그가 성공하였는지, 실패하였는지, 진행 중인지 판별한다.
	ALLEGRO_FONT *font = al_create_builtin_font(); //알레그로 빌트인 폰트를 생성한다.

	//0. 초기 설정
	set_dialog(dialog, lv); //지역 변수로 생성된 다이얼로그 배열에 각 ai에 맞춘 대화록을 설정한다.
	al_set_target_backbuffer(sub_disp);	//비트맵들을 그릴 디스플레이 버퍼를 서브디스플레이로 바꾼다.

	al_set_timer_count(turn_timer, 0); //턴 타이머의 현재 값을 0으로 바꾼다.
	al_start_timer(turn_timer); //턴 타이머의 동작을 시작한다.

	//일반적인 그래픽 처리 루틴 (while을 사용한)을 사용하지 않는다. al_draw_multiline_text()의 실행 속도가 느려서 이벤트 입력 시간과 어긋나기 때문이다.

	//(서브)디스플레이 검은색으로 초기화
	al_clear_to_color(RGB_BLACK);
	//1. 지금까지 진행했던 대화 + ai의 현재 질문을 출력한다.
	for (i = (num - 2 < 0) ? 0 : num - 2; i <= num; i++) {
		al_draw_multiline_textf(font, RGB_WHITE, 20, 20 + row * 140, 300, 15, 0, "%s : %s", p2->name, dialog[i].question); //질문을 출력한다.
		if (i < num) { //num은 현재 함수가 실행 된 횟수이다. 저번 횟수까지 실행한 대화는 사용자가 선택한 답과 그에 맞춘 ai의 대답이 저장되어 있지만 이번 질문에서는 아직 저장되기 전이다.
			al_draw_multiline_textf(font, RGB_WHITE, 20, 60 + row * 140, 300, 15, 0, "%s : %s", p1->name, dialog[i].answer[saved_ans[i]]); //사용자의 대답을 출력한다.
			al_draw_multiline_textf(font, RGB_WHITE, 20, 100 + row * 140, 300, 15, 0, "%s : %s", p2->name, dialog[i].reaction[saved_ans[i]]); //사용자의 대답에 대한 ai의 응답을 출력한다.
			row++; //row는 현재 화면에 출력된 대화의 갯수이다. y좌표의 균등한 높이 차이를 유지하기 위해서 사용된다. 
		}
	}
	//대답 목록을 출력한다.
	al_draw_rectangle(10, 600, 330, 780, RGB_WHITE, 0);
	al_draw_multiline_textf(font, RGB_WHITE, 20, 620, 300, 15, 0, "1 : %s", dialog[num].answer[0]);
	al_draw_multiline_textf(font, RGB_WHITE, 20, 670, 300, 15, 0, "2 : %s", dialog[num].answer[1]);
	al_draw_multiline_textf(font, RGB_WHITE, 20, 720, 300, 15, 0, "3 : %s", dialog[num].answer[2]);
	al_flip_display();

	//2.현재 질문에 대한 플레이어의 대답을 기다린다.
	while (!answered) {
		//그래픽은 처리하지 않지만 이벤트를 기다린다.
		al_wait_for_event(queue, &event);
		switch (event.type) {
		case ALLEGRO_EVENT_TIMER:
			if (event.timer.source == turn_timer && al_get_timer_count(turn_timer) >= 10) { //10초안에 답을 선택하지 않으면
				saved_ans[num] = rand() % 3; //랜덤으로 답을 선택하여 진행한다.
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

	//3. 대답에 대한 점수를 계산, 현재 질문에 대한 대답과 응답을 출력한다.
	score += dialog[num].score[saved_ans[num]];
	al_draw_multiline_textf(font, RGB_WHITE, 20, 60 + row * 140, 300, 15, 0, "%s : %s", p1->name, dialog[num].answer[saved_ans[num]]);
	al_flip_display();
	al_rest(0.5);
	al_draw_multiline_textf(font, RGB_WHITE, 20, 100 + row * 140, 300, 15, 0, "%s : %s", p2->name, dialog[num].reaction[saved_ans[num]]);
	al_flip_display();
	num++; //대화 횟수 증가시키기

	//4. 대답 처리 후 점수가 0점보다 작으면 다이얼로그 실패
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
		num = 0; //다음 게임을 위해 초기화
		score = 20; //다음 게임을 위해 초기화
		state = Failed; //다이얼로그 실패 상태
	}
	//4. 대답 처리 후 더 이상 출력할 대화 목록이 없으면 다이얼로그 성공
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
		num = 0; //다음 게임을 위해 초기화
		score = 20; //다음 게임을 위해 초기화
		state = Successed; //다이얼로그 성공 상태
	}
	//5.함수 종료를 준비한다.
	al_stop_timer(turn_timer); //타이머 정지
	al_destroy_font(font); //폰트를 메모리에서 릴리즈
	al_set_target_backbuffer(disp); //타겟 버퍼를 메인 디스플레이로 전환한다.
	return state; //다이얼로그 상태를 반환한다.
}

void set_dialog(DIALOG dialog[], int lv) {
	switch (lv) {
	case 0: //데카르트
		//질문 1
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

		//질문 2
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

		//질문 3
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

		//질문 4
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

		//질문 5
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
	case 1: //헤이덤
		//질문 1
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

		//질문 2
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

		//질문 3
		dialog[2].question = "Even when your kind appears to triumph… Still we rise again. And do you know why? ";

		dialog[2].answer[0] = "...";
		dialog[2].score[0] = 5;
		dialog[2].reaction[0] = "It is because the Order is born of a realization. We require no creed.";

		dialog[2].answer[1] = "Only the Freedom is matter";
		dialog[2].score[1] = -10;
		dialog[2].reaction[1] = "Freedom is an invitation for chaos.";

		dialog[2].answer[2] = "what does the Templars want?";
		dialog[2].score[2] = 10;
		dialog[2].reaction[2] = "All we need is that the world be as it is. And THIS is why the Templars will never be destroyed!”";

		//질문 4
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

		//질문 5
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
	case 2: //렉스
		//질문 1
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

		//질문 2
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

		//질문 3
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

		//질문 4
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

		//질문 5
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
	//보드판 전체를 검사했을 때
	for (i = 0; i < 45; i++) {
		if (board[i] == White)
			white++;//보드판의 말의 흰색이라면 흰 색 말의 수를 증가시킨다.
		else if (board[i] == Black)
			black++;//보드판의 말의 검은색이라면 검은 색 말의 수를 증가시킨다.
	}
	if (white == 0 && black == 0)
		return Draw; //보드판에 흰색 말과 검은 말이 하나도 없다면 무승부 판정
	else if (white == 0) //보드판에 흰색 말이 하나도 없을 때 검은색이 승리했다는 판정을 내린다
		return Black_win;
	else if (black == 0) //보드판에 검은 말이 하나도 없을 때 흰색이 승리했다는 판정을 내린다.
		return White_win;
	else
		return No_win; //위의 경우에 다 해당되지 않으면 아직 승패가 나지 않았다.
}