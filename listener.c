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
	int n, s;
	struct sockaddr_in local;
	char buffer[256];
	int done;

	for(int a = 0; a < 256; a++) {
		buffer[a] = '\0';
	}
	
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	printf("Trying to connect...\n");

	memset(&local, 0, sizeof(struct sockaddr_in));
	local.sin_family = AF_INET;
	local.sin_port = htons(1058);
	//local.sin_addr.s_addr = inet_addr("10.0.0.1");

	if (connect(s, (struct sockaddr *)&local, sizeof(struct sockaddr)) == -1) {
        	perror("connect");
        	exit(1);
    	}
	
	printf("Connected.\n");

	done = 0;
	do {
		n = recv(s, buffer, sizeof(buffer), 0);
		if (n <= 0) {
			if (n < 0) { 
				perror("recv");
			}
			done = 1;
		}
		/*
		if (n < 0) {
			perror("recv");
		}
		else {
			printf("Server closed connection\n");
			exit(1);
		}
		*/

		printf("Received: %s:%d\n", buffer, n);

		for(int a = 0; a < 256; a++) {
			buffer[a] = '\0';
		}
	} while(!done);

	return 0;
} 
