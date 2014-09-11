#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <event.h>
#include <unistd.h>

typedef struct {
	int num;
	int maxnum;
}cb_param_t;

void say_hello(evutil_socket_t fd, short what, void *arg) 
{
       cb_param_t *param = (cb_param_t*)arg;
        printf("%s%s%s%s\n",
            (what&EV_TIMEOUT) ? " timeout" : "",
            (what&EV_READ)    ? " read" : "",
            (what&EV_WRITE)   ? " write" : "",
            (what&EV_SIGNAL)  ? " signal" : "");
       printf("num==%d, maxnum=%d\n", param->num++, param->maxnum);
}

 
int main(int argc, const char* argv[])
{
  struct event *ev;
  struct timeval tv = {1,0};
  struct event_base *base;

  int fd[2];
  pipe(fd);

  int pid = 0;
  close(fd[1]);
  base = event_base_new();
  cb_param_t *param = malloc(sizeof(cb_param_t));
  param->num=0;
  param->maxnum=100;
  ev = event_new(base, -1, EV_TIMEOUT | EV_PERSIST, say_hello, param);
  event_add(ev, &tv);
  event_base_dispatch(base);
  return 0;
}
