#include <stdio.h>
#include <event.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

int pp[2];
struct event *evw;
struct event *evr;
struct event_base *base;

int
main() {

    const char **methods = event_get_supported_methods();
    struct event_config *cfg = NULL;
    struct event_base *base = NULL;
    const char *backend;
    int n_methods = 0;


    backend = methods[0];
    while (*methods != NULL) {
    		printf("%s\n", methods[n_methods]);
            ++methods;
            ++n_methods;
    }

    cfg = event_config_new();
    assert(cfg != NULL);

    assert(event_config_avoid_method(cfg, backend) == 0);
    event_config_set_flag(cfg, EVENT_BASE_FLAG_IGNORE_ENV);

    base = event_base_new_with_config(cfg);
    if (n_methods > 1) {
            assert(backend != event_base_get_method(base));
    } else {
    }

   event_config_free(cfg);
}

