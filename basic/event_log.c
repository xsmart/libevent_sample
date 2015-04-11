#include <event2/event.h>

#define LOG_LEVEL   EVENT_LOG_ERR

const char *
msg_level(int severity)
{
    /* use switch for better choice */
    if (severity == EVENT_LOG_DEBUG)
        return "debug";
    else if (severity == EVENT_LOG_MSG)
        return "msg";
    else if (severity == EVENT_LOG_WARN)
        return "warn";
    else if (severity == EVENT_LOG_ERR)
        return "error";

    return NULL;
}

#define S(severity) msg_level(severity)

/* 1. you must not call libevent functions again. */
/* 2. EVENT_LOG_DBG May be useless to you */
void
log_event (int severity, const char *msg)
{
    if (severity >= LOG_LEVEL) {
        printf ("[%s] %s\n", S(severity), msg);
    }
}

int main ()
{
    /* log setting */
    event_set_log_callback(log_event);
    event_base_free (NULL);

    /* set to the default log behavior */
    event_set_log_callback(NULL);
    event_base_free (NULL);

}
