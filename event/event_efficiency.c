#include <stdio.h>
#include <event2/event.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#ifndef WIN32
#include <sys/resource.h>
#include <sys/time.h>
#endif
#include <base/base.h>

#ifndef WIN32

/* https://msdn.microsoft.com/en-us/library/windows/desktop/ms739169%28v=vs.85%29.aspx */

static void set_resource_limits()
{
    struct rlimit rlim_new;

    //set the maximum size open files
    rlim_new.rlim_cur = rlim_new.rlim_max = 1000000;
    if (setrlimit(RLIMIT_NOFILE, &rlim_new) != 0) {
        perror("set RLIMIT_NOFILE failed");
        exit(EXIT_FAILURE);
    }
}

#endif

#define PAIR_NUM 20000
evutil_socket_t fds[2*PAIR_NUM];
int pp[2];
struct event_base *base;

void
func_cb (evutil_socket_t fd, short ev, void *arg)
{
    printf ("socket pair index <%d> called\n", (int)arg);
    fflush(NULL);
}

void*
thread_func (void *arg)
{
    int i;
    for (i=0; i<PAIR_NUM; i++) {
        send (fds[2*i+1], "h", 1, 0);
        msleep (1);
    }
    printf ("thread done\n");
    return NULL;
}

int
main()
{
#ifdef WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    int cc = WSAStartup(wVersionRequested, &wsaData);
    assert (cc==0);
#else
    set_resource_limits ();
#endif

    struct event_config *config = event_config_new ();
    assert (NULL != config);
    event_config_set_flag (config, EVENT_BASE_FLAG_STARTUP_IOCP);
    base = event_base_new_with_config(config);
    assert (NULL != base);

    int i = 0;
    for (i=0; i<PAIR_NUM; i++) {
        struct event *ev =NULL;
        int cc = 0;
        cc = evutil_socketpair (AF_UNIX, SOCK_STREAM, 0, fds+2*i);
        if (cc) {
            printf ("the number of socket pair <%d>\n", i);
            break;
        }

        ev = event_new (base, fds[2*i], EV_READ, func_cb, (void*)i);
        assert (NULL != ev);
        cc = event_add (ev, NULL);
        if (cc != 0) {
            printf ("the number of socket pair <%d>\n", i);
            break;
        }
    }

    struct timeval time_begin = {0,0};
    struct timeval time_end = {0,0};
    gettimeofday(&time_begin, NULL);
    thread_t thread;
    i_thread_create (&thread, THREAD_ATTR_DEFAULT, thread_func, NULL);
    event_base_dispatch (base);
    gettimeofday(&time_end, NULL);

    printf ("use <%d> seconds to handle all events\n", time_end.tv_sec-time_begin.tv_sec);
    event_base_free (base);

    return 0;
}
