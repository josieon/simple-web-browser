#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	int sockd;			// 통신 소켓
	struct sockaddr_in Server;	// 구조체
	int returnStatus;		// 리턴상태값
	
	// 사용법
	if(argc < 3)
	{
		fprintf(stderr, "Usage: %s <ip address> <port number>\n", argv[0]);
		exit(1);
	}

	// 소켓 생성. HTTP1.1 TCP 사용
	sockd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockd == -1)
	{
		fprintf(stderr, "Could not create socket!\n");
		exit(1);
	}
	else
	{
		printf("Trying %s...\n", argv[1]);
	}

	// 서버구조체 초기화
	Server.sin_family = AF_INET;
	Server.sin_addr.s_addr = inet_addr(argv[1]);
	Server.sin_port = htons(atoi(argv[2]));

	// 서버로 연결 시도
	returnStatus = connect(sockd, (struct sockaddr*)&Server, sizeof(Server));

	if (returnStatus == -1)
	{
		fprintf(stderr, "Could not connect to server!\n");
		exit(1);
	}
	else
	{
		printf("Connected to %s.\n", argv[1]);
		printf("Escape character is '^]'.\n");
	}

	char buf[128];	// 전송 버퍼
	char line[50];	// 입력 버퍼
// 우분투상에서는 LF(Line Feed)를 이용하지만 HTTP에서 CRLF를 이용.
// 따라서 버퍼를 구분하여 선언.

	bzero(buf, 128);
	while((returnStatus = read(0, line, 50)) != 1)
	{
		line[strlen(line)-1] = '\0';	// '\n'을 떼준다.
		strcat(line, "\r\n");		// '\r\n'을 붙여준다.
		strcat(buf, line);		// 전송버퍼에 입력버퍼값 붙여줌.
	}
	strcat(buf, "\r\n");			// 개행만 입력받았을시 루프 종료, CRLF 붙여줌.
	write(sockd,buf,strlen(buf));
	while(read(sockd ,buf,128) != 0)	// 소켓으로부터 데이터를 읽어 출력해주고, 버퍼 초기화 반복.
	{
		write(1, buf, 128);
		memset(buf, 0, 128);
	}
	printf("Connection closed by foreign host.\n");	//Timeout되면 연결이 끊어지고 read함수가 0을 return하여 루프 종료.
	close(sockd);	//소켓 반환
	return 0;
}