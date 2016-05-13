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

/*
I get this code idea/implementation from
stackoverflow.com/questions/2605182/when-binding-a
-client-tcp-socket-to-a-specific-local-port-winsock-so-reuse
*/

int main(void) 
{
	int s, s2;
	int t;
	int broadcast = 1;
	struct sockaddr_in local;
	struct sockaddr_in remote;
	char buffer[256];

	for(int a = 0; a < 256; a++) {
		buffer[a] = '\0';
	}
	
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	int enable = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		perror("setsockopt(SO_REUSEADDR) failed");
	}

	if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1) {
		perror("setsockopt (SO_BROADCAST)");
		exit(1);
	}

	printf("Trying to connect...\n");

	memset(&local, 0, sizeof(struct sockaddr_in));
	local.sin_family = AF_INET;
	local.sin_port = htons(1058);
	local.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(bind(s, (struct sockaddr *)&local, sizeof(struct sockaddr)) == -1) {
		perror("binding");
		exit(1);	
	}

	if (listen(s, 5) == -1) {
		perror("listen");
		exit(1);
	}


	t = sizeof(remote);
	if ((s2 = accept(s, (struct sockaddr *)&remote, &t)) == -1) {
		perror("accept");
		exit(1);
	}

	printf("Connected.\n");

	while(printf("Enter a line: "), fgets(buffer, 256, stdin), !feof(stdin)) {
		if (strcmp(buffer, "q\n") == 0) {
			printf("quitting...");
			close(s);
			exit(1);
		}

		if (send(s2, buffer, (strlen(buffer)-1), 0) == -1) {
		    perror("send");
		    exit(1);
		}
	}

	close(s);
	
	//send(s, buffer, strlen(buffer), 0);
	//recv(s, buffer, sizeof(buffer), 0);
	//printf("%s\r\n", buffer);
}
