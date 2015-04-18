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

int pp[2];
struct event_base *base;

void
func_cb (evutil_socket_t fd, short ev, void *arg)
{

}

#define PAIR_NUM 20000

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

    base = event_base_new();
    assert (NULL != base);
    struct event *ev =NULL;

    int count = 0;
    while (1) {
        evutil_socket_t fd[2];
        int cc = 0;
        /* ON windows, there is no limit. */
        //cc = evutil_socketpair (AF_UNIX, SOCK_STREAM, 0, fd);  /* bottle is 204240*2 */
        //cc = pipe (pp);       /* there is unlimit, Linux tired to die. */
        if (cc) {
            printf ("the number of socket pair <%d>\n", count);
            break;
        }

#if 0
        fd[0] = socket (AF_INET, SOCK_STREAM, 0);
        fd[1] = socket (AF_INET, SOCK_STREAM, 0);
        if (fd[0]==INVALID_SOCKET || fd[1]==INVALID_SOCKET) {
            printf ("the number of socket pair <%d>\n", count);
            break;
        }
#endif


#if 1
        ev = event_new (base, fd[0], EV_READ, func_cb, NULL);
        assert (NULL != ev);
        cc = event_add (ev, NULL);
        if (cc != 0) {
            printf ("the number of socket pair <%d>\n", count);
            break;
        }

        ev = event_new (base, fd[1], EV_WRITE, func_cb, NULL);
        assert (NULL != ev);
        /* ON windows, there is no limit. */
        cc = event_add (ev, NULL);  /* 326625 is the bottle, It is also the epoll bottle */
        if (cc != 0) {
            printf ("the number of socket pair <%d>\n", count);
            break;
        }
        printf ("add fd success on <%d, %d>\n", fd[0], fd[1]);
#endif
        count++;
    }

    event_base_dispatch (base);
    assert (1==2);
    event_base_free (base);

    return 0;
}
