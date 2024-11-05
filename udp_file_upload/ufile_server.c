#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sock;
	FILE * fp;
	char buf[BUF_SIZE];
	int read_cnt, file_size;
	int write_cnt;
	
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;
	
	if (argc != 2) {
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	fp = fopen("hgu.jpg", "rb"); 
	serv_sock = socket(PF_INET, SOCK_DGRAM, 0);   
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));
	
	if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
	
	// Receive request meesage from client 
	clnt_adr_sz = sizeof(clnt_adr);
	recvfrom(serv_sock, buf, BUF_SIZE, 0, 
				(struct sockaddr*)&clnt_adr, &clnt_adr_sz);
	printf("Client's message: %s\n", buf);

	// TODO: Send file size to client 
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char file_size_str[20];
	snprintf(file_size_str, sizeof(file_size_str), "%d", file_size);
	sendto(serv_sock, file_size_str, sizeof(file_size_str), 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);


	// TODO: Send file data to client 
	while ((read_cnt = fread((void*)buf, 1, BUF_SIZE, fp)) > 0) {
		sendto(serv_sock, buf, read_cnt, 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);
		printf("Sent %d bytes\n", read_cnt);
	}
	sendto(serv_sock, "", 0, 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);
	
	
	recvfrom(serv_sock, buf, BUF_SIZE, 0, 
				(struct sockaddr*)&clnt_adr, &clnt_adr_sz); 
	printf("Message from client: %s \n", buf);

	fclose(fp);
	close(serv_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
