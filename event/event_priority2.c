#include <stdio.h>
#include <event2/event.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

int pp[2];
struct event *ev1;
struct event *ev2;
struct event *ev3;

/* when to change the pending event to active ? 
 * After same priority avtive events done, check.
 */

void
cb_1(evutil_socket_t fd, short evtype, void *arg)
{
    sleep (2);
    printf("cb_1\n");
}

void
cb_2(evutil_socket_t fd, short evtype, void *arg)
{
    printf("cb_2\n");
}

void
cb_3(evutil_socket_t fd, short evtype, void *arg)
{
    sleep (2);
    printf("cb_3\n");
}

int
main()
{
    pipe(pp);

    /* invoke event_config_set_max_dispatch_interval to change the default behavior. */
    struct event_config *config = event_config_new ();
    struct timeval interval = {1,0};
    event_config_set_max_dispatch_interval (config, &interval, 1, 1);
    struct event_base *base = event_base_new_with_config(config);
    event_base_priority_init(base, 20); 
    
    ev1 = event_new(base, -1, EV_TIMEOUT | EV_PERSIST, cb_1, NULL);
    ev2 = event_new(base, -1, EV_TIMEOUT | EV_PERSIST, cb_2, NULL);
    ev3 = event_new(base, -1, EV_TIMEOUT | EV_PERSIST, cb_3, NULL);

    /* If they are different priority, low priority will have no chance to exec. 
     * No matter event_config_set_max_dispatch_interval
     */
    event_priority_set(ev1, 18);    /* 0-19 */
    event_priority_set(ev3, 11);

    struct timeval tv = {1,0};
    event_add(ev1, &tv);
    event_add(ev2, &tv);
    event_add(ev3, &tv);

    /* 20 */
    printf ("event_base_get_npriorities()==%d\n",event_base_get_npriorities (base));
    event_base_dispatch(base);

    return 0;
}

