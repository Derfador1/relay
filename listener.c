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

#define MAXN 256

#define SOCK_PATH "/home/sparadis/file"

/*
I get this code idea/implementation from
stackoverflow.com/questions/2605182/when-binding-a
-client-tcp-socket-to-a-specific-local-port-winsock-so-reuse
*/
int main(void) 
{
	int bytes, sock, len;
	struct sockaddr_un local;
	char buffer[MAXN];
	int done;

	for(int counter = 0; counter < MAXN; counter++) {
		buffer[counter] = '\0';
	}
	
	if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	printf("Trying to connect...\n");

	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, SOCK_PATH);
	len = strlen(local.sun_path) + sizeof(local.sun_family);

	if (connect(sock, (struct sockaddr *)&local, len) == -1) {
        	perror("connect");
		exit(1);
    	}
	
	printf("Connected.\n");

	done = 0;
	do {
		bytes = recv(sock, buffer, sizeof(buffer), 0);
		if (bytes <= 0) {
			if (bytes < 0) { 
				perror("recv");
			}
			done = 1;
		}

		if (strlen(buffer) <= 0) {
			printf("Connection cut.\n");
			close(sock);
			exit(1);
		}

		printf("Received: %s", buffer);

		for(int counter = 0; counter < MAXN; counter++) {
			buffer[counter] = '\0';
		}
	} while(!done);

	printf("\n");

	close(sock);

	return 0;
} 
