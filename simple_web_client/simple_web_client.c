#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#define PORT 80
#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock;
	char *host_name;
	char ip_addr[16];
	char buf[BUF_SIZE];
	int recv_cnt;
	struct hostent *host;
	struct sockaddr_in serv_adr;

	if (argc != 2) {
		printf("Usage : %s <addr>\n", argv[0]);
		exit(1);
	}

	host_name = argv[1];

	// TODO: Get IP address and copy to ip_addr (char array)
	// 도메인 이름을 IP 주소로 변환
	host = gethostbyname(host_name);
	if (!host)
		error_handling("gethost... error");
	// 네트워크 주소를 문자열 형식의 IP 주소로 변환
	strcpy(ip_addr, inet_ntoa(*(struct in_addr*)host->h_addr_list[0]) );

	// ip_addr is string for IP address 
	printf("IP addr: %s \n", ip_addr); 
	
	sock = socket(PF_INET, SOCK_STREAM, 0);   
	if (sock == -1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	
	// TODO: Set server address 
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(ip_addr);
	serv_adr.sin_port = htons(PORT);
	
	if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error!");
	else
		printf("Connected to %s \n", host_name);
	
	// Send HTTP Request message to server 
	sprintf(buf, "GET / HTTP/1.0\r\nHost: %s\r\nUser-Agent: HTMLGET \r\n\r\n", host_name);
	printf("Client HTTP Message: \n%s", buf);
	
	// TODO: Transmit HTTP message to server 
	write(sock, buf, sizeof(buf));

	// Receive HTTP Response message from server 
	memset(buf, 0, strlen(buf));
	while (recv_cnt = read(sock, buf, BUF_SIZE-1) > 0) {
		printf("%s", buf);
	}
	printf("\nComplete \n");
	
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
