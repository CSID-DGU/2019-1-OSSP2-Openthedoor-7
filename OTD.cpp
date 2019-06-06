#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <string.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <pthread.h>
#include <semaphore.h>

int user_idx;

int score=80;
int first(char * myId, char * myPwd);
int previousScore();
void ScoreUpdate();
int rankGet();
void quit(char * name, int user_idx);

int main(int argc, char **argv){

char myId[100];
char myPwd[100];
int user_idx = -1;
user_idx = first(myId, myPwd);
printf("login complete");

quit(myId,user_idx);
}


int first(char * myId, char * myPwd){
        int sock, sign;
	char buff[1000];
	struct sockaddr_in serv_addr;
	sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(buff, 0x00, sizeof(buff));
	memset(myId, 0x00, sizeof(myId));
	memset(myPwd, 0x00, sizeof(myPwd));
	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("13.124.167.29");
	serv_addr.sin_port = htons(3090);


	system("clear");
	printf("\n\n\t -------LOGIN--------\n\n");
	printf("\tID : ");
	scanf("%s", myId);

	printf("\tPW : ");
	scanf("%s", myPwd);
	printf("\n\n");

	while (1) {
		printf("\t[1]signup [2]signin : ");
		scanf("%d", &sign);

		if (sign == 1) {
			//for signup
			// serv_addr.sin_addr.s_addr = inet_addr("13.209.29.192");

			if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
				//printf("connect error");
			}

			sprintf(buff, "%c|%s|%s", '1', myId, myPwd);
			write(sock, buff, sizeof(buff)); // send socket to server
											 //printf("%s\n",buff);
			memset(buff, 0x00, sizeof(buff)); // empty buffer
			read(sock, buff, sizeof(buff)); // read socket from server
			printf("\tuser_idx : ");
			printf("%s\n", buff);
			if (buff != NULL) {
				printf("\t sign up success !");
				system("clear");
				user_idx=first(myId, myPwd);
                                break;
			}
			
			//break;

		}
		else if (sign == 2) {
			//for login
			if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
				//printf("connect error");
                        }

			sprintf(buff, "%c|%s|%s", '2', myId, myPwd);
			write(sock, buff, sizeof(buff)); // send socket to server
			//printf("%s\n",buff);
			memset(buff, 0x00, sizeof(buff)); // empty buffer
			read(sock, buff, sizeof(buff)); // read socket from server

			printf("\tuser_idx : ");
			printf("%s\n", buff);

			if (buff != NULL) {
				printf("\t success !");
			}
			break;

		}
		else {
			printf("It is wrong");
		}
	}
	close(sock);
	user_idx = (atoi)(buff);
	return user_idx;
}

void quit(char * name, int user_idx)
{

	char end;
	int previous_score = -1;
int 
	//tcsetattr(0, TCSANOW, &back_attr); //TCSANOW는 즉시속성을 변경을 의미,

	previous_score = previousScore();

	if (score > previous_score) {
		ScoreUpdate();
		rank = rankGet();
		//firstScore = firstScoreGet();

		//if (rank != 1) {
		//	printf("\n\n\t축하합니다. %s님이 최고점수 %d점을 달성했습니다.\n", name, score);
		//	printf("\n\t당신은 현재 %d등 입니다. 1등의 %d점 도전해보세요! \n\n", rank, firstScore);
		//}
		 
			printf("\n\n\t축하합니다. %s님이 최고점수 %d점을 달성했습니다.\n", name, score);
			printf("\n\t당신은 현재 1등 입니다. 본인의 최고 점수를 갱신해보세요! \n\n");
		
	}
	else {
		//rank get
		printf("\n\n\t실력이 많이 녹슬었군요. 과거의 %s님은 점수 %d점을 달성했습니다.\n\n", name, previous_score);
		printf("\n\t 한번 더 도전해서, 과거의 자신을 뛰어 넘어 보세요!\n");
	}
	//save the lifes (rare items)
	//saveLifes(user_idx);
	printf("\n\n\tpress enter to end the game!\n");
	while (1) {
		end = getchar();
		if (end == '\n')break;
	}
	//set_cursor(True);
	//tcsetattr(0, TCSANOW, &back_attr); //TCSANOW는 즉시속성을 변경을 의미, 터미널 세팅을 되돌리기
	//system("clear"); //입력창이 다 밑으로 내려가서 이걸로하면 다시위로감

	return;
}

int previousScore() {
	int sock, previous_score;
	char buff[1000];
	struct sockaddr_in serv_addr;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(buff, 0x00, sizeof(buff));

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("13.124.167.29");
	serv_addr.sin_port = htons(3090);

	if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
		//printf("connect error");
	}
	//socre get
	sprintf(buff, "%c|%d", '5', user_idx);
	write(sock, buff, sizeof(buff));
	//printf("%s\n", buff);
	memset(buff, 0x00, sizeof(buff));
	read(sock, buff, sizeof(buff));
	//printf("%s\n", buff);
	//printf("current score : ");
	previous_score = (atoi)(buff);
	close(sock);
	return previous_score;
}

void ScoreUpdate()
{
	int sock;
	char buff[1000];
	struct sockaddr_in serv_addr;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(buff, 0x00, sizeof(buff));

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("13.124.167.29");
	serv_addr.sin_port = htons(3090);

	if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
		//  printf("connect error");
	}
	memset(buff, 0x00, sizeof(buff));
	sprintf(buff, "%c|%d|%d", '6', score, user_idx);
	write(sock, buff, sizeof(buff));
	//printf("%s\n", buff);
	memset(buff, 0x00, sizeof(buff));
	read(sock, buff, sizeof(buff));
	close(sock);
}



