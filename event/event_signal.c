#include <event2/event.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>

void
sigstop (evutil_socket_t fd, short ev, void *arg){
    printf ("enter %s\n", __func__);
    sleep (100);
    printf ("leave %s\n", __func__);
}

void
sigint (evutil_socket_t fd, short ev, void *arg){
    printf ("enter %s\n", __func__);
    sleep (100);
    sleep (100);
    printf ("leave %s\n", __func__);
}

int main ()
{
    struct event_base *base = event_base_new();
    assert (NULL != base);

    struct event *ev = NULL;
    ev = event_new (base, SIGINT, EV_SIGNAL, sigint, NULL);
    event_add (ev, NULL);

    ev = event_new (base, SIGTSTP, EV_SIGNAL, sigstop, NULL);
    event_add (ev, NULL);

    event_base_dispatch(base);
}
