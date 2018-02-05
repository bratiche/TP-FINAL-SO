#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define TCP_PORT 4444 
#define UNIX_PATH_MAX   108

int connection_handler(int connection_fd);

void* subway(void * args){
	 int *cl_pointer=(int *)args;
	 int cl=*cl_pointer;
	connection_handler(cl);
}

int connection_handler(int connection_fd)
{
 int nbytes;
 char buffer[256];
char buf[100];
int rc;

 nbytes = read(connection_fd, buffer, 256);
 buffer[nbytes] = 0;

// printf("MESSAGE FROM CLIENT: %s\n", buffer);
// nbytes = snprintf(buffer, 256, "hello from the server");
// write(connection_fd, buffer, nbytes);
 int cl=connection_fd;
  while(1){
    while ( (rc=read(cl,buf,sizeof(buf))) > 0) {
      printf("read %u bytes: %.*s\n", rc, rc, buf);
    }
    if (rc == -1) {
      perror("read");
    }
    else if (rc == 0) {
      printf("EOF\n");
      close(cl);
      return 0;
    }
 }
 close(connection_fd);
 return 0;
}

int main(void)
{
 struct sockaddr_in addr;
 int socket_fd, connection_fd;
 socklen_t address_length;

 socket_fd = socket(AF_INET, SOCK_STREAM, 0);
 if(socket_fd < 0)
 {
  printf("socket() failed\n");
  return 1;
 }

  addr.sin_port = htons(TCP_PORT);
  addr.sin_addr.s_addr = 0;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_family = AF_INET;
 //unlink("./demo_socket");

 /* start with a clean address structure
 memset(&address, 0, sizeof(struct sockaddr_un));

 address.sun_family = AF_UNIX;
 snprintf(address.sun_path, UNIX_PATH_MAX, "./demo_socket");
*/
 if(bind(socket_fd,
         (struct sockaddr *) &addr,
         sizeof(struct sockaddr_in)) != 0)
 {
  printf("bind() failed\n");
  return 1;
 }

 if(listen(socket_fd, 5) != 0)
 {
  printf("listen() failed\n");
  return 1;
 }

 while((connection_fd = accept(socket_fd,
                               (struct sockaddr *) &addr,
                               &address_length)) > -1)
 {/*
  child = fork();
  if(child == 0)
  {
   /* now inside newly created connection handling process */
  /* return connection_handler(connection_fd);
  }

  /* still inside server process */
  //close(connection_fd);
    printf("AGARRE UN NUEVO LOCO");
    pthread_t thread;
    pthread_create(&thread,NULL,subway,&connection_fd);
    //pthread_join(thread,NULL);

 }

 close(socket_fd);
 //unlink("./demo_socket");
 return 0;
}
