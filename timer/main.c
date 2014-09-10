#include <stdio.h>
#include <sys/time.h>
#include <event.h>
#include <unistd.h>

void say_hello(evutil_socket_t fd, short what, void *arg) 
{
       const char *data = arg;
        printf("%s %d:%s%s%s%s\n",
		data,
            (int) fd,
            (what&EV_TIMEOUT) ? " timeout" : "",
            (what&EV_READ)    ? " read" : "",
            (what&EV_WRITE)   ? " write" : "",
            (what&EV_SIGNAL)  ? " signal" : "");
}

 
int main(int argc, const char* argv[])
{
  struct event *ev;
  struct timeval tv = {3,0};
  struct event_base *base;

  int fd[2];
  pipe(fd);

  int pid = 0;
/*
  if((pid = fork()) == 0) {
	close(fd[0]);
	dup2(fd[1], STDOUT_FILENO);
	printf("child\n");
	//write(fd[1], "child", 5);
  }
*/
  close(fd[1]);
  base = event_base_new();
  ev = event_new(base, fd[0], EV_TIMEOUT | EV_READ, say_hello, "get an event on socket");
  event_add(ev, &tv);
  event_base_dispatch(base);
  return 0;
}
