#include <event2/event.h>
#include <stdio.h>
#include <stdlib.h>

/* 1. you must not call libevent functions */
/* 2. you should not return control back to event */
/* 3. we can not find manner to make fatal_err called */
void
fatal_err (int err)
{
    printf ("fatal err called\n");
    exit (-1);
}

int main ()
{
    event_set_fatal_callback(fatal_err);

    struct event_base *base = event_base_new();

    event_base_loopexit (base, NULL);
    event_base_free (base);

    return 0;
}
