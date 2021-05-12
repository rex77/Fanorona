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
	MYSQL conn; //db����
	MYSQL *connection; //Ŀ�ؼ��� �ڵ�

	char id[20] = { 0 };
	char pw[20] = { 0 };
	char query[160] = { 0 };
	mysql_init(&conn);

	connection = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3307, (char *)NULL, 0); //MySQLĿ�ؼ��� Ȯ��

	printf("ȸ�������� �����մϴ�. ");
	printf("���ο� ���̵� �Է��ϼ��� : ");
	fgets(id, 50, stdin); //���̵� �����ϴ� ���ۿ� �Է��� �޴´�.
	CHOP(id); //���͸� �����Ѵ�.

	printf("���ο� �н����带 �Է��ϼ��� : ");
	fgets(pw, 50, stdin); //�н����带 �����ϴ� ���ۿ� �Է��� �޴´�.
	CHOP(pw); //���͸� �����Ѵ�.

	sprintf(query,"INSERT INTO user_info (UserID, UserPW, Name, Win, Tie, Lose) VALUES ('%s', '%s', '%s', %d, %d, %d)", id, pw, local_user.name, local_user.win, local_user.tie, local_user.lose);
	//���� ��Ʈ���� �̿��� �Է¹��� ������ �������� ������ ���ۿ� sql������ �����Ѵ�.
	if (mysql_query(connection, query)) { //������ �����ϰ� ��ȯ�� (0 �Ͻ� ����, �ٸ� ��ȣ�� �� ����)�� ���� ����/�������θ� ����ϰ� Ŀ�ؼ��� �ݴ´�.
		printf("ȸ������ ����\n"); //���и� ǥ���Ѵ�.
		mysql_close(&conn); //Ŀ�ؼ��� �ݴ´�.
		return false; //login_or_register() �Լ��� ó���� ���� ���� ���� ���θ� ��ȯ�Ѵ�.
	}
	
	printf("ȸ������ ����\n"); //������ ǥ���Ѵ�.
	strcpy(logined_id , id);
	mysql_close(&conn); //Ŀ�ؼ��� �ݴ´�.
	return true; //login_or_register() �Լ��� ó���� ���� ���� ���� ���θ� ��ȯ�Ѵ�.
}

bool login_user(void) {
	MYSQL conn; //db����
	MYSQL *connection; //Ŀ�ؼ��� �ڵ�
	MYSQL_RES *result; //���
	MYSQL_ROW sql_row;

	char id[20] = {0}; 
	char pw[20] = {0}; 
	mysql_init(&conn);

	connection = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3307, (char *)NULL, 0);

	printf("���̵� : ");
	fgets(id, 50, stdin); 
	CHOP(id);

	printf("�н����� : ");
	fgets(pw, 50, stdin);
	CHOP(pw);
	
	if (mysql_query(connection, "select * from user_info"))  //mysql_query�� ���� �̿��ؼ� ���� ����
		printf("MYSQL query error\n");
	result = mysql_store_result(connection); //���� ��ȸ ������� ����

	while ((sql_row = mysql_fetch_row(result)) != NULL) { //��������� �� �྿ ���ڵ带 �����ͼ� ���Ѵ�.
		if (strcmp(id, sql_row[0]) == 0 && strcmp(pw, sql_row[1]) == 0){ //id�ʵ�� pw�ʵ带 ���ؼ� �α��� ���θ� ����
			printf("�α��� ����\n");
			strcpy(logined_id, id); //�� ���α׷����� �α����� logined_id�� id���� ������ ���� ������ ����ϴ� ������ �����Ѵ�.
			strcpy(local_user.name, sql_row[2]); //Name
			local_user.win = atoi(sql_row[3]); //Win
			local_user.tie = atoi(sql_row[4]); //Tie
			local_user.lose = atoi(sql_row[5]); //Lose
			if (sql_row[6] != NULL) { //���� (�����ʻ����� ���������� �ִٸ� ����� ������ ��ΰ� �����, �⺻�� NULL)
				local_user.toon = al_load_bitmap(sql_row[6]);
				if (local_user.toon == NULL)
					local_user.toon = al_load_bitmap("Data/Graphic/Player_toon.png"); //���� ������ �����Ǿ��ٴ��� �ϴ� ������ ��������Ʈ�� �ҷ����µ� ������ ����ٸ� �⺻ ������ �ҷ���
			}
			mysql_close(&conn); //Ŀ�ؼ��� �����Ѵ�.
			return true; //�α��� ������ ��ȯ�Ѵ�.
		}
	}
	printf("�α��� ����\n"); //���� return true�� ������ ���ߴٸ� ������ ���� ����̴�.
	mysql_close(&conn);
	return false; //�α��� ���и� ��ȯ�Ѵ�.
}

void update_user_name(char *name) {
	MYSQL conn; //db����
	MYSQL *connection; //Ŀ�ؼ��� �ڵ�
	char query[160] = { 0 };
	mysql_init(&conn);

	connection = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3307, (char *)NULL, 0); //Ŀ�ؼ� Ȯ��
	sprintf(query,"UPDATE user_info SET Name='%s' WHERE UserId = '%s'", name, logined_id); //���̵� ����Ű�� ���ڵ忡 �̸��� ������Ʈ�ϴ� �������� �����Ѵ�.

	if (mysql_query(connection, query)) //MYSQL�� ���� ��û �� ���� Ȯ��
		printf("login error\n");
	//������� �ʿ����.
	mysql_close(&conn);//Ŀ�ؼ��� �����ϰ� �Լ��� �����Ѵ�.
}

void update_user_toon(char *path) {
	MYSQL conn; //db����
	MYSQL *connection; //Ŀ�ؼ��� �ڵ�
	char query[160] = { 0 };
	char path_copy[100] = { 0 };
	mysql_init(&conn);

	connection = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3307, (char *)NULL, 0);
	backslash_to_slash(strcpy(path_copy,path)); //MYSQL�� ���־� ��Ʃ��� �� ��� �� '\'�� �ν����� ���ϴ� ������ �߻��Ѵ�. �̽������� �������� ����ص� �ذ���� �ʾ� ������� �����쿡�� �Ȱ��� �ν��ϴ� �������� �����Ѵ�.
	sprintf(query, "UPDATE user_info SET Toon='%s' WHERE UserID = '%s'", path_copy, logined_id); //���̵� ����Ű�� ���ڵ忡 �н����� ������Ʈ�ϴ� �������� �����Ѵ�. ���� �Լ����� PATH�� ��ȿ���� üũ�ϱ� ������ ���⼭�� üũ�� �� �ʿ䰡 ����.

	if (mysql_query(connection, query)) //���� �� ������ Ȯ��
		printf("login error\n");

	mysql_close(&conn); //Ŀ�ؼ��� �����ϰ� �Լ��� ��ȯ�Ѵ�.
}

void update_user_result(void) {
	MYSQL conn; //db����
	MYSQL *connection; //Ŀ�ؼ��� �ڵ�
	char query[160] = { 0 };
	char path_copy[100] = { 0 };
	mysql_init(&conn);

	connection = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3307, (char *)NULL, 0);
	sprintf(query, "UPDATE user_info SET Win = %d, Tie = %d, Lose = %d WHERE UserID = '%s'", local_user.win, local_user.tie, local_user.lose, logined_id);
	//�α��� �Ǿ� �ִ� ���� ������ ���̵� �̿��� ������ ���ε��Ѵ�.

	if (mysql_query(connection, query)) //���� ���� �ڵ�
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