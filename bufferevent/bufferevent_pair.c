#include <event2/bufferevent.h>
#include <event2/event.h>
#include <assert.h>

struct bufferevent *pair[2];
int count;

void*
thread_func (void *arg)
{
    while (1) {
        bufferevent_lock (pair[0]);
        printf ("count == %d\n", count);
        count++;
        sleep(1);
        bufferevent_unlock (pair[0]);
    }
}

int main ()
{
    int cc;

    /* use pthreads */
    cc = evthread_use_pthreads();
    assert (cc == 0);

    /* must specify BEV_OPT_THREADSAFE */
    struct event_base *base = event_base_new ();
    cc = bufferevent_pair_new (base, BEV_OPT_THREADSAFE, pair);
    assert (cc == 0);

    /* set callback function */

    pthread_t tid1;
    pthread_t tid2;
    pthread_create (&tid1, NULL, thread_func, NULL);
    pthread_create (&tid2, NULL, thread_func, NULL);

    pthread_join (tid1, NULL);
    pthread_join (tid2, NULL);

    fflush (NULL);
    //event_base_dispatch (base);

    return 0;
}
