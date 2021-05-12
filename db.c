#include <my_global.h>
#include <mysql.h>
#include <stdio.h>
#include <string.h>
#include "user.h"
#pragma warning(disable:4996)

#define DB_HOST "127.0.0.1"
#define DB_USER "root"
#define DB_PASS "MyNewPass"
#define DB_NAME "fanorona"
#define CHOP(x) x[strlen(x) - 1] = '\0' 

bool register_user(void) {
	MYSQL conn; //db정보
	MYSQL *connection; //커넥션의 핸들

	char id[20] = { 0 };
	char pw[20] = { 0 };
	char query[160] = { 0 };
	mysql_init(&conn);

	connection = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3307, (char *)NULL, 0); //MySQL커넥션을 확립

	printf("회원가입을 진행합니다. ");
	printf("새로운 아이디를 입력하세요 : ");
	fgets(id, 50, stdin); //아이디를 저장하는 버퍼에 입력을 받는다.
	CHOP(id); //엔터를 제거한다.

	printf("새로운 패스워드를 입력하세요 : ");
	fgets(pw, 50, stdin); //패스워드를 저장하는 버퍼에 입력을 받는다.
	CHOP(pw); //엔터를 제거한다.

	sprintf(query,"INSERT INTO user_info (UserID, UserPW, Name, Win, Tie, Lose) VALUES ('%s', '%s', '%s', %d, %d, %d)", id, pw, local_user.name, local_user.win, local_user.tie, local_user.lose);
	//포맷 스트링을 이용해 입력받은 정보를 바탕으로 쿼리용 버퍼에 sql쿼리를 저장한다.
	if (mysql_query(connection, query)) { //쿼리를 실행하고 반환값 (0 일시 정상, 다른 번호일 시 실패)을 비교해 실패/성공여부를 출력하고 커넥션을 닫는다.
		printf("회원가입 실패\n"); //실패를 표시한다.
		mysql_close(&conn); //커넥션을 닫는다.
		return false; //login_or_register() 함수의 처리를 위해 성공 실패 여부를 반환한다.
	}
	
	printf("회원가입 성공\n"); //성공을 표시한다.
	strcpy(logined_id , id);
	mysql_close(&conn); //커넥션을 닫는다.
	return true; //login_or_register() 함수의 처리를 위해 성공 실패 여부를 반환한다.
}

bool login_user(void) {
	MYSQL conn; //db정보
	MYSQL *connection; //커넥션의 핸들
	MYSQL_RES *result; //결과
	MYSQL_ROW sql_row;

	char id[20] = {0}; 
	char pw[20] = {0}; 
	mysql_init(&conn);

	connection = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3307, (char *)NULL, 0);

	printf("아이디 : ");
	fgets(id, 50, stdin); 
	CHOP(id);

	printf("패스워드 : ");
	fgets(pw, 50, stdin);
	CHOP(pw);
	
	if (mysql_query(connection, "select * from user_info"))  //mysql_query의 값을 이용해서 에러 검출
		printf("MYSQL query error\n");
	result = mysql_store_result(connection); //쿼리 조회 결과값을 저장

	while ((sql_row = mysql_fetch_row(result)) != NULL) { //결과값에서 한 행씩 레코드를 가져와서 비교한다.
		if (strcmp(id, sql_row[0]) == 0 && strcmp(pw, sql_row[1]) == 0){ //id필드와 pw필드를 비교해서 로그인 여부를 결정
			printf("로그인 성공\n");
			strcpy(logined_id, id); //본 프로그램에서 로그인은 logined_id에 id값을 저장해 이후 쿼리에 사용하는 것으로 구현한다.
			strcpy(local_user.name, sql_row[2]); //Name
			local_user.win = atoi(sql_row[3]); //Win
			local_user.tie = atoi(sql_row[4]); //Tie
			local_user.lose = atoi(sql_row[5]); //Lose
			if (sql_row[6] != NULL) { //만약 (프로필사진을 변경한적이 있다면 변경된 사진의 경로가 저장됨, 기본값 NULL)
				local_user.toon = al_load_bitmap(sql_row[6]);
				if (local_user.toon == NULL)
					local_user.toon = al_load_bitmap("Data/Graphic/Player_toon.png"); //만약 파일이 삭제되었다던지 하는 이유로 스프라이트를 불러오는데 문제가 생겼다면 기본 사진을 불러옴
			}
			mysql_close(&conn); //커넥션을 종료한다.
			return true; //로그인 성공을 반환한다.
		}
	}
	printf("로그인 에러\n"); //만약 return true를 만나지 못했다면 에러가 생긴 경우이다.
	mysql_close(&conn);
	return false; //로그인 실패를 반환한다.
}

void update_user_name(char *name) {
	MYSQL conn; //db정보
	MYSQL *connection; //커넥션의 핸들
	char query[160] = { 0 };
	mysql_init(&conn);

	connection = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3307, (char *)NULL, 0); //커넥션 확립
	sprintf(query,"UPDATE user_info SET Name='%s' WHERE UserId = '%s'", name, logined_id); //아이디가 가리키는 레코드에 이름을 업데이트하는 쿼리문을 저장한다.

	if (mysql_query(connection, query)) //MYSQL에 쿼리 요청 후 오류 확인
		printf("login error\n");
	//결과값은 필요없다.
	mysql_close(&conn);//커넥션을 종료하고 함수를 종료한다.
}

void update_user_toon(char *path) {
	MYSQL conn; //db정보
	MYSQL *connection; //커넥션의 핸들
	char query[160] = { 0 };
	char path_copy[100] = { 0 };
	mysql_init(&conn);

	connection = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3307, (char *)NULL, 0);
	backslash_to_slash(strcpy(path_copy,path)); //MYSQL과 비주얼 스튜디오 간 통신 중 '\'를 인식하지 못하는 문제가 발생한다. 이스케이프 시퀀스를 사용해도 해결되지 않아 대안으로 윈도우에서 똑같이 인식하는 슬래쉬로 변형한다.
	sprintf(query, "UPDATE user_info SET Toon='%s' WHERE UserID = '%s'", path_copy, logined_id); //아이디가 가리키는 레코드에 패스값을 업데이트하는 쿼리문을 저장한다. 선행 함수에서 PATH값 유효성을 체크하기 때문에 여기서는 체크를 할 필요가 없다.

	if (mysql_query(connection, query)) //쿼리 후 오류를 확인
		printf("login error\n");

	mysql_close(&conn); //커넥션을 종료하고 함수를 반환한다.
}

void update_user_result(void) {
	MYSQL conn; //db정보
	MYSQL *connection; //커넥션의 핸들
	char query[160] = { 0 };
	char path_copy[100] = { 0 };
	mysql_init(&conn);

	connection = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3307, (char *)NULL, 0);
	sprintf(query, "UPDATE user_info SET Win = %d, Tie = %d, Lose = %d WHERE UserID = '%s'", local_user.win, local_user.tie, local_user.lose, logined_id);
	//로그인 되어 있는 로컬 유저의 아이디를 이용해 전적을 업로드한다.

	if (mysql_query(connection, query)) //에러 검출 코드
		printf("SQL error\n");

	mysql_close(&conn);
}

void backslash_to_slash(char *str) {
	int i = 0;
	for (i = 0; str[i] != '\0'; i++) {
		if(str[i] == '\\' )
			str[i] = '/';
	}
}