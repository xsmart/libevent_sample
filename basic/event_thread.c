#include <event2/event.h>
#include <stdio.h>
#include <stdlib.h>

void
fatal_err (int err)
{
    printf ("fatal err called\n");
    exit (-1);
}

int main ()
{
    /* just set numbers of  fucntions to libevent */
    /* It is necessary before lock libevent structure */
    evthread_use_pthreads();
    //evthread_use_windows_threads();

    /* report some lock error */
    evthread_enable_lock_debugging();

    struct event_base *base = event_base_new();

    event_base_loopexit (base, NULL);
    event_base_free (base);

    return 0;
}
