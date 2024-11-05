#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 32
#define SEND_BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sd;
	FILE *fp;
	
	char file_name[BUF_SIZE];
	char buf[BUF_SIZE];
	char send_buf[SEND_BUF_SIZE];
	int read_cnt;
	int read_size = 0;
	struct sockaddr_in serv_adr;
	if (argc != 4) {
		printf("Usage: %s <IP> <port> <file name> \n", argv[0]);
		exit(1);
	}

	strcpy(file_name, argv[3]);
	fp = fopen(argv[3], "rb");
	sd = socket(PF_INET, SOCK_STREAM, 0);   

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	connect(sd, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
	
	// TODO: Send file name to server 
	write(sd, file_name, sizeof(file_name));

	while ((read_cnt = fread((void*)send_buf, 1, SEND_BUF_SIZE, fp)) > 0) {
		write(sd, send_buf, read_cnt);
		read_size += read_cnt;
		printf("Sent %d bytes\n", read_size);
	}

	

	printf("Send total %d bytes \n", read_size);

	// TODO: shutdown 
	shutdown(sd, SHUT_WR);

	// TODO: read complete message from server 
	int str_len = read(sd, buf, BUF_SIZE - 1);
	if (str_len == -1) {
		error_handling("read() error!");
	}
	buf[str_len] = 0;
	printf("Message from server: %s \n", buf);

	fclose(fp);
	close(sd);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}