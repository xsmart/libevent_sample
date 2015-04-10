#include <event2/event.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

struct event_base *base;
int pp[2];

void
print_features (struct event_base *base)
{
	const char *method=event_base_get_method(base);
    printf ("--- method <%s> ---\n", method);
    
	int cnt = event_base_get_features(base);
    if (cnt & EV_FEATURE_ET) {
        /* see man for more information about edge-triggered and level-triggered */
        printf("support edge-triggered events with EV_ET\n");
    }
	if(cnt & EV_FEATURE_O1) {
	    /* poll and select is O(n)  */
		printf("having one event triggered among many is an O(1) operation.\n");
	}
	if(cnt & EV_FEATURE_FDS) {
	    /* epoll not support it */
		printf("allows file descriptors as well as sockets. \n");
	}
	if (cnt & EV_FEATURE_EARLY_CLOSE) {
	    /* detect connection close without the necessity of reading all the pending data. */
        printf ("EV_FEATURE_EARLY_CLOSE\n");
	}
	printf ("that is all\n\n");


}

void
poll_feature_test()
{
    struct event_config *config = event_config_new();
    event_config_avoid_method (config, "select");
    event_config_avoid_method (config, "epoll");

    struct event_base *base = event_base_new_with_config (config);
    print_features(base);
    event_base_free(base);
}

void
select_feature_test()
{
    struct event_config *config = event_config_new();
    event_config_avoid_method (config, "poll");
    event_config_avoid_method (config, "epoll");

    struct event_base *base = event_base_new_with_config (config);
    print_features(base);
    event_base_free(base);
}

void feature_test ()
{
    /* supported async */
    const char ** methods = event_get_supported_methods(); 
    if (methods) {
        int i = 0;
        while(methods[i]) {
            printf ("method %d : <%s>\n", i+1, methods[i]);
            i++;
        }
    }

    /* get the default async method */
	base = event_base_new();
    const char * bb = event_base_get_method(base);
    printf("default method is <%s>\n", bb);     /* epoll */

	print_features(base);

	event_base_free(base);
}

int
main() {
    feature_test();
    poll_feature_test();
    select_feature_test();
	pipe(pp);
	return 0;
}

