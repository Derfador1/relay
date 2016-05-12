#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SOCK_PATH "echo_socket"

int main(void) 
{
	int ret, s;
	struct sockaddr_in local;
	char buffer[256];
	
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	printf("Trying to connect...\n");

	memset(&local, 0, sizeof(struct sockaddr_in));
	local.sin_family = AF_INET;
	local.sin_port = htonl(1058);
	local.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(bind(s, (struct sockaddr *)&local, sizeof(struct sockaddr)) == -1) {
		perror("binding");
		exit(1);	
	}

	send(s, buffer, strlen(buffer), 0);
	recv(s, buffer, sizeof(buffer), 0);
	printf("%s\r\n", buffer);
}
