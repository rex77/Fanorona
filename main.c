#include "main.h"
#include "graphics.h"
#include "test.h"
#include "game.h"
#include "audio.h"

//WELCOME TO FANORONA
int main(void) {
	int op = 0; //타이틀 함수의 반환값 : 사용자의 선택을 받는 함수

	must_init(al_init(),"initiallize"); //알레그로 라이브러리 초기화
	must_init(al_init_font_addon(),"font addon"); //폰트 애드온 초기화
	must_init(al_init_ttf_addon(),"ttf addon"); //트루타입 폰트 확장자 지원 애드온 초기화
	must_init(al_init_image_addon(),"image addon");
	must_init(al_init_native_dialog_addon(), "native dialog addon"); //네이티브 다이얼로그 애드온 초기화
	must_init(al_init_acodec_addon(), "audio codec addon"); //오디오 코덱 애드온 초기화
	must_init(al_init_primitives_addon(), "primitives addon"); //도형 그리기 애드온 초기화 (직접 좌표를 지정해 그리는 애드온)

	//알레그로에서 입력장치를 사용할 수 있도록 설치
	must_init(al_install_keyboard(), "keyboard installer"); //키보드 기능
	must_init(al_install_audio(), "audio installer"); //사운드 기능
	must_init(al_reserve_samples(2), "reserve sample"); //사운드 믹서 생성

	disp = al_create_display(WINDOW_WIDTH, WINDOW_HEIGHT); //알레그로용 디스플레이를 생성, 일반 커맨드라인 창과 다름
	must_init(disp,"display");
	timer = al_create_timer(1.0 / FPS); 
	must_init(timer,"timer");
	queue = al_create_event_queue(); 
	must_init(queue, "queue"); //이벤트 큐가 생성되었는지 검사

	//이벤트 큐에 이벤트를 발생시키는 개체들을 등록
	al_register_event_source(queue, al_get_keyboard_event_source()); //키보드 이벤트 검사 가능
	al_register_event_source(queue, al_get_display_event_source(disp)); //메인 디스플레이 검사 가능
	al_register_event_source(queue, al_get_timer_event_source(timer)); //타이머 이벤트 검사 가능

	al_start_timer(timer);

	//사용자 정의 함수
	//프로그램 실행 시에 필요한 개체들을 초기화 (그래픽 소스, 오디오 소스, 유저 정보)
	must_init(init_graphics(),"load images");
	must_init(init_audio(),"audio files");
	init_user(); 
	al_set_display_icon(disp, preload_icon); //디스플레이 아이콘 설정
	
	/*
	* 프로그램의 메인 로직
	* 메인 반복문 내에서 타이틀 함수를 호출하고, 타이틀 함수에서 사용자의 입력을 받으며 함수 반환값을 이용해 각 상황에 맞는 함수를 호출 한다.
	*/
	while (true) {
		//타이틀 화면 표시하는 함수
		op = show_title();
		//타이틀이 끝나면 선택한 메뉴에 맞는 함수를 호출한다.
		switch (op) { //각 case값은 enum Menu값
		case Campaign:
			play_game(Campaign); //게임을 캠페인 모드로 시작한다.
			break;
		case Freeplay:
			play_game(Freeplay); //프리플레이 모드로 게임을 시작
			break;
		case Two_player:
			play_game(Two_player); //2p모드로 게임을 시작
			break;
		case Tutorial:
			show_tutorial(); //튜토리얼 페이지를 호출
			break;
		case Option:
			show_options(); //게임 설정을 호출
			break;
		case Profile:
			if (login) { //bool login.. 로그인 여부에 따라 호출하는 함수가 달라진다.
				show_profile();
			} else {
				login = login_or_register();
			}
			break;
		case Exit: 
			if (al_show_native_message_box(NULL, "Fanorona", "Closing Fanorona", "Exit the game?", NULL, ALLEGRO_MESSAGEBOX_YES_NO) == 1)
				exit(0); //프로그램 종료
		}
	}

	destroy_graphics(); 
	destroy_audios();
	al_destroy_display(disp);
	al_destroy_timer(timer); 
	al_destroy_event_queue(queue); 
	return 0;
}

void must_init(bool func, const char *str) {
	if (!func) { //NULL이 아니면
		printf("couldn't load %s\n", str); //couldn't load 개체
	}
}