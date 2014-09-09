#include <stdio.h>
#include <sys/time.h>
#include <event.h>

void say_hello(evutil_socket_t fd, short what, void *arg) 
{
  printf("Hello\n");
}
 
int main(int argc, const char* argv[])
{
  struct event *ev;
  struct timeval tv = {3,0};
  struct event_base *base;

  base = event_base_new();
  ev = event_new(base, -1, EV_TIMEOUT | EV_PERSIST, say_hello, "hello");
 
  event_add(ev, &tv);
  event_base_dispatch(base);
 
  return 0;
}
