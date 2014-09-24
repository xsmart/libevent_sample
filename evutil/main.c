#include <event.h>
#include <assert.h>
#include <event.h>

int
main(void) {
//	char buf[1024];
	char *buf;
	long long num = evutil_strtoll("123nihao", &buf, 10);		/* do not need free. */
	printf("num is =%d\n", num);
	printf("bug is =%s\n", buf);
}
