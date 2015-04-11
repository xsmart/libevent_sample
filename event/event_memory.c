#include <event2/event.h>
#include <stdio.h>
#include <stdlib.h>

void*
i_malloc (size_t len)
{
    printf ("i_malloc called\n");
    return malloc(len);
}

void
i_free (void *ptr)
{
    printf ("i_free called\n");
    if (NULL==ptr)
        return;
    free(ptr);
}

void *
i_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

int main ()
{
    event_set_mem_functions (i_malloc, i_realloc, i_free);

    struct event_base *base = event_base_new();
    event_base_free (base);
    printf ("______\n");
    libevent_global_shutdown(); /* nothing to free */
    return 0;
}
