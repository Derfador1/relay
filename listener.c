#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/un.h>

#define SOCK_PATH "echo_socket"

/*
I get this code idea/implementation from
stackoverflow.com/questions/2605182/when-binding-a
-client-tcp-socket-to-a-specific-local-port-winsock-so-reuse
*/
int main(void) 
{
	int n, s, len;
	struct sockaddr_un local;
	char buffer[256];
	int done;

	for(int a = 0; a < 256; a++) {
		buffer[a] = '\0';
	}
	
	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	printf("Trying to connect...\n");

	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, SOCK_PATH);
	len = strlen(local.sun_path) + sizeof(local.sun_family);

	if (connect(s, (struct sockaddr *)&local, len) == -1) {
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

		if (strcmp(buffer, "q\n") == 0) {
			printf("Connection cut.\n");
			close(s);
			exit(1);
		}
		printf("Received: %s", buffer);

		for(int a = 0; a < 256; a++) {
			buffer[a] = '\0';
		}
	} while(!done);

	printf("\n");

	close(s);

	return 0;
} 
