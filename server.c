#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 10010
#define QUEUE_SZ 5
#define BUF_SZ 4096

typedef struct mystruct {
	int client_socket;
} MYSTRUCT;

void* handler (void* arg) {
		MYSTRUCT* sa = (MYSTRUCT*)arg;
		char buffer [4096];

		int bytes = 0;
    if((bytes = read(sa->client_socket, buffer, BUF_SZ)) <= 0){
        close(sa->client_socket);
    }
		printf("Message received\n");

		/* write message to socket */
		write(sa->client_socket, buffer, strlen(buffer));
		printf("3-Message sent\n");

    close(sa->client_socket);  /* close connection */
		printf("4-Conn closed\n");

	return NULL;
}

int main (int argc, char** argv) {

	int s, b, l, fd, sa, bytes, on = 1;
  char buf[BUF_SZ]; /* content read */
  struct sockaddr_in channel_srv; /* hold's IP address */
  
  /* Build address structure to bind to socket. */
  memset(&channel_srv, 0, sizeof(channel_srv)); /* zero channel */
  channel_srv.sin_family = AF_INET;
  channel_srv.sin_addr.s_addr = htonl(INADDR_ANY);
  channel_srv.sin_port = htons(PORT);
  
  /* Passive open. Wait for connection. */
  s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /* create socket */
  if (s < 0) {
      perror("Usage: socket");
      exit(EXIT_FAILURE);
  }
	printf("Socket Created\n");
  
  b = bind(s, (struct sockaddr *) &channel_srv, sizeof(channel_srv));
  if (b < 0) {
      perror("Usage: bind");
      exit(EXIT_FAILURE);
  }
	printf("Bind done\n");

  l = listen(s, QUEUE_SZ); /* specify queue size */
  if (l < 0) {
      perror("Usage: listen");
      exit(EXIT_FAILURE);
  }
	printf("Listening..\n");

	/* Socket is now set up and bound. Wait for connection and process it. */
  while (1) {
    sa = accept(s, 0, 0); /* block for connection request */
    if (sa < 0) {
        perror("Accept");
        continue;
    }
		printf("Conn accepted\n");
		
		pthread_t tid;
		MYSTRUCT info;
		info.client_socket = sa;
		int ok_ = pthread_create(&tid, NULL, handler, (void*) &info);
		if (ok_ != 0) {
			perror("Usage: pthread_create");
			exit(EXIT_FAILURE);
		}

		// make it so server doesn't block until request is handled
		pthread_detach(tid);

  }

	return 0;
}
