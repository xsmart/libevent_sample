#include <event2/event.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>

/* you can add event on other thread, but the execution thread 
 * must be the thread same with event loop
 */

void
fatal_err (int err)
{
    printf ("fatal err called\n");
    exit (-1);
}
struct event_base *base;

void*
thread_func(void *arg)
{
    event_base_dispatch(base);
    /* Only one event_base_loop can run on each event_base at once */
    assert (1==2);
}

void
sigint (evutil_socket_t fd, short ev, void*arg)
{
    printf ("func == %s\n", __func__);
}

void
sigtstp (evutil_socket_t fd, short ev, void*arg)
{
    printf ("func == %s\n", __func__);
}

int main ()
{
    /* just set numbers of fucntions to libevent */
    /* It is necessary before lock libevent structure */
    evthread_use_pthreads();
    //evthread_use_windows_threads();

    base = event_base_new();

    struct event *ev = event_new (base, SIGINT, EV_SIGNAL | EV_PERSIST, sigint, NULL);
    event_add (ev, NULL);
    ev = event_new (base, SIGTSTP, EV_SIGNAL | EV_PERSIST, sigtstp, NULL);
    event_add (ev, NULL);

    pthread_t thread;
    pthread_create (&thread, NULL, thread_func, NULL);

    event_base_dispatch (base);
    event_base_free (base);
    return 0;
}
