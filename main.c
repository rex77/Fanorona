#include "main.h"
#include "graphics.h"
#include "test.h"
#include "game.h"
#include "audio.h"

//WELCOME TO FANORONA
int main(void) {
	int op = 0; //Ÿ��Ʋ �Լ��� ��ȯ�� : ������� ������ �޴� �Լ�

	must_init(al_init(),"initiallize"); //�˷��׷� ���̺귯�� �ʱ�ȭ
	must_init(al_init_font_addon(),"font addon"); //��Ʈ �ֵ�� �ʱ�ȭ
	must_init(al_init_ttf_addon(),"ttf addon"); //Ʈ��Ÿ�� ��Ʈ Ȯ���� ���� �ֵ�� �ʱ�ȭ
	must_init(al_init_image_addon(),"image addon");
	must_init(al_init_native_dialog_addon(), "native dialog addon"); //����Ƽ�� ���̾�α� �ֵ�� �ʱ�ȭ
	must_init(al_init_acodec_addon(), "audio codec addon"); //����� �ڵ� �ֵ�� �ʱ�ȭ
	must_init(al_init_primitives_addon(), "primitives addon"); //���� �׸��� �ֵ�� �ʱ�ȭ (���� ��ǥ�� ������ �׸��� �ֵ��)

	//�˷��׷ο��� �Է���ġ�� ����� �� �ֵ��� ��ġ
	must_init(al_install_keyboard(), "keyboard installer"); //Ű���� ���
	must_init(al_install_audio(), "audio installer"); //���� ���
	must_init(al_reserve_samples(2), "reserve sample"); //���� �ͼ� ����

	disp = al_create_display(WINDOW_WIDTH, WINDOW_HEIGHT); //�˷��׷ο� ���÷��̸� ����, �Ϲ� Ŀ�ǵ���� â�� �ٸ�
	must_init(disp,"display");
	timer = al_create_timer(1.0 / FPS); 
	must_init(timer,"timer");
	queue = al_create_event_queue(); 
	must_init(queue, "queue"); //�̺�Ʈ ť�� �����Ǿ����� �˻�

	//�̺�Ʈ ť�� �̺�Ʈ�� �߻���Ű�� ��ü���� ���
	al_register_event_source(queue, al_get_keyboard_event_source()); //Ű���� �̺�Ʈ �˻� ����
	al_register_event_source(queue, al_get_display_event_source(disp)); //���� ���÷��� �˻� ����
	al_register_event_source(queue, al_get_timer_event_source(timer)); //Ÿ�̸� �̺�Ʈ �˻� ����

	al_start_timer(timer);

	//����� ���� �Լ�
	//���α׷� ���� �ÿ� �ʿ��� ��ü���� �ʱ�ȭ (�׷��� �ҽ�, ����� �ҽ�, ���� ����)
	must_init(init_graphics(),"load images");
	must_init(init_audio(),"audio files");
	init_user(); 
	al_set_display_icon(disp, preload_icon); //���÷��� ������ ����
	
	/*
	* ���α׷��� ���� ����
	* ���� �ݺ��� ������ Ÿ��Ʋ �Լ��� ȣ���ϰ�, Ÿ��Ʋ �Լ����� ������� �Է��� ������ �Լ� ��ȯ���� �̿��� �� ��Ȳ�� �´� �Լ��� ȣ�� �Ѵ�.
	*/
	while (true) {
		//Ÿ��Ʋ ȭ�� ǥ���ϴ� �Լ�
		op = show_title();
		//Ÿ��Ʋ�� ������ ������ �޴��� �´� �Լ��� ȣ���Ѵ�.
		switch (op) { //�� case���� enum Menu��
		case Campaign:
			play_game(Campaign); //������ ķ���� ���� �����Ѵ�.
			break;
		case Freeplay:
			play_game(Freeplay); //�����÷��� ���� ������ ����
			break;
		case Two_player:
			play_game(Two_player); //2p���� ������ ����
			break;
		case Tutorial:
			show_tutorial(); //Ʃ�丮�� �������� ȣ��
			break;
		case Option:
			show_options(); //���� ������ ȣ��
			break;
		case Profile:
			if (login) { //bool login.. �α��� ���ο� ���� ȣ���ϴ� �Լ��� �޶�����.
				show_profile();
			} else {
				login = login_or_register();
			}
			break;
		case Exit: 
			if (al_show_native_message_box(NULL, "Fanorona", "Closing Fanorona", "Exit the game?", NULL, ALLEGRO_MESSAGEBOX_YES_NO) == 1)
				exit(0); //���α׷� ����
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
	if (!func) { //NULL�� �ƴϸ�
		printf("couldn't load %s\n", str); //couldn't load ��ü
	}
}