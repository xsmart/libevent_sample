#include <event.h>
#include <assert.h>
#include <string.h>

int
add_data_to_buf(void) {
	struct evbuffer *bufin =  evbuffer_new();
	struct evbuffer *bufout = evbuffer_new();

	evbuffer_add_printf(bufin, "%s/%d", "hello", 1);
	size_t len = evbuffer_get_length(bufin);
	assert(7 == len);

	evbuffer_add_buffer(bufout, bufin);		/* read from @bufin to @bufout. */
	assert(evbuffer_get_length(bufin) == 0);
	assert(evbuffer_get_length(bufout) == 7);

	evbuffer_add(bufout, "nihao", 5);	/* you can set num=6, include the '\0' */
	assert(evbuffer_get_length(bufout) == 12);
	evbuffer_add_printf(bufout, "%s/%d", "word", 1);
	assert(evbuffer_get_length(bufout) == 18);	/* hello/1nihaoword/1 */

	evbuffer_prepend(bufout, "ppppp", 5);
	assert(evbuffer_get_length(bufout) == 23);	/* hello/1nihaoword/1 */

	char *b = evbuffer_pullup(bufout, -1);
	assert(strcmp(b, "ppppphello/1nihaoword/1") == 0);

	evbuffer_drain(bufout, -1);
	assert(evbuffer_get_length(bufout) == 0);

}

int
remove_data_from_buf(void) {
	struct evbuffer *bufin =  evbuffer_new();
	struct evbuffer *bufout = evbuffer_new();

	evbuffer_add(bufin,  "bufin",  5);
	evbuffer_add(bufout, "bufout", 6);
	evbuffer_remove_buffer(bufin, bufout, 5);
	assert(evbuffer_get_length(bufout)==11);
	assert(evbuffer_get_length(bufin)==0);
}

int
main(void) {
	remove_data_from_buf();
}




