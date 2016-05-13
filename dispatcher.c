#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/un.h>
#include <sys/stat.h>

#define MAXN 256

#define SOCK_PATH "/home/sparadis/file"

/*
I get this code idea/implementation from
stackoverflow.com/questions/2605182/when-binding-a
-client-tcp-socket-to-a-specific-local-port-winsock-so-reuse
*/

int main(void) 
{
	int sock;
	int accept_s;
	int max;
	int bytes;
	int enable = 1;
	int len;
	char buffer[MAXN];

	struct sockaddr_un local;
	struct sockaddr_storage remote1;

	socklen_t addrlen;

	fd_set master;
	fd_set read_fds;
	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	for(int a = 0; a < 256; a++) {
		buffer[a] = '\0';
	}

	printf("Setting up...\n");
	
	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sock < 0) {
		perror("socket");
		exit(1);
	}

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
		perror("setsockopt(SO_REUSEADDR) failed");
	}

	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, SOCK_PATH);
	unlink(local.sun_path);
	len = strlen(local.sun_path) + sizeof(local.sun_family);

	if(bind(sock, (struct sockaddr *)&local, len) == -1) {
		perror("binding");
		close(sock);	
		exit(1);
	}

	chmod(SOCK_PATH, 0777);

	if (listen(sock, 5) == -1) {
		perror("listen");
		exit(1);
	}

	/*
	the following code was gotten with help from MSG Simpson and found at
	https://github.com/Ragnok/relay/blob/master/dispatcher.c
	*/

	FD_SET(sock, &master);
	FD_SET(STDIN_FILENO, &master);

	max = sock;

	printf("Enter a line: ");
	fflush(stdout);

	while(1) {
		read_fds = master;
		if(select(max + 1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(1);
		}

		int current;
		for (current = 0; current <= max; current++) {
			if(FD_ISSET(current, &read_fds)) {
				if(current == sock) {
					addrlen = sizeof(remote1);
					accept_s = accept(sock, (struct sockaddr *)&remote1, &addrlen);

					if(accept_s == -1) {
						perror("accept");
					}
					else {
						FD_SET(accept_s, &master);
						if(accept_s > max) {
							max = accept_s;
						}
					}
				}
				else if(current == STDIN_FILENO){
					if ((bytes = read(current, buffer, sizeof(buffer))) > 0) {
						if (strncmp(buffer, "q\n", 2) == 0) {
							printf("quitting..\n");
							close(sock);
							exit(1);
						}

						for(current = 0; current <= max; current++) {
							if (FD_ISSET (current, &master) && current != STDIN_FILENO && current != sock) {
								if (send(current, buffer, bytes, 0) == -1) {
									perror("send");
								}
							}
						}
						printf("Enter a line: ");
						fflush(stdout);
					}
				}
			}
		}
	}

	close(sock);
}
