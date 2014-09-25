/*
  This exmple program provides a trivial server program that listens for TCP
  connections on port 9995.  When they arrive, it writes a short message to
  each client connection, and closes each connection once it is flushed.

  Where possible, it exits cleanly in response to a SIGINT (ctrl-c).
*/


#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#ifndef WIN32
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>
#endif

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/bufferevent.h>

#include <event2/thread.h>
#include <pthread.h>

#ifndef USE_X_PRINT
#define aPrt printf
#define dPrt printf
#define ePrt printf
#define bgPrt printf
#define edPrt printf
#else 
#include "hx_debug.h"
#endif

static int flag = 0;
static const char MESSAGE[] = "Hello, World!\n";

static const int PORT = 9995;

static void listener_cb(struct evconnlistener *, evutil_socket_t,
    struct sockaddr *, int socklen, void *);
static void conn_writecb(struct bufferevent *, void *);
static void conn_readcb(struct bufferevent *, void *);
static void conn_eventcb(struct bufferevent *, short, void *);
static void signal_cb(evutil_socket_t, short, void *);

int
main(int argc, char **argv)
{
	int i = 0;
	struct event_base *base;
	struct evconnlistener *listener;
	struct event *signal_event;

	struct sockaddr_in sin;
#ifdef WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif

	base = event_base_new();
	if (!base) {
		fprintf(stderr, "Could not initialize libevent!\n");
		return 1;
	}
	/* 支持线程 */
	evthread_use_pthreads();

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);

	/* LEV_OPT_THREADSAFE 支持 listener 多线程 */
	listener = evconnlistener_new_bind(base, listener_cb, (void *)base,
	    LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE|LEV_OPT_THREADSAFE, -1,
	    (struct sockaddr*)&sin,
	    sizeof(sin));

	if (!listener) {
		fprintf(stderr, "Could not create a listener!\n");
		return 1;
	}

	/* 信号 */
	signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);

	if (!signal_event || event_add(signal_event, NULL)<0) {
		fprintf(stderr, "Could not create/add a signal event!\n");
		return 1;
	}

	// 打印event正在使用的方法
	char **a = event_get_supported_methods();
	for( i=0; a[i]!=NULL; i++ ) {
		printf( "[%s]\n", a[i] );
	}

	event_base_dispatch(base);

	evconnlistener_free(listener);
	event_free(signal_event);
	event_base_free(base);

	printf("done\n");
	return 0;
}

static void
listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
    struct sockaddr *sa, int socklen, void *user_data)
{
	struct event_base *base = user_data;
	struct bufferevent *bev;
	printf( "begin listener_cb \n" );
	/* sleep(3); */

	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE );
	/* bev = bufferevent_socket_new(base, fd,BEV_OPT_THREADSAFE ); */
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(base);
		return;
	}

	// 设置超时时间 
	// 客户端连接上服务器，服务端在规定是否没有读到则调用conn_eventcb进行超时处理
	struct timeval tv_read = { 5, 0 }, tv_write = { 5, 0 };
	bufferevent_set_timeouts (bev, &tv_read, &tv_write);

	/* BEV_OPT_THREADSAFE-支持 buffevent 多线程 */
	int ret = bufferevent_enable(bev, BEV_OPT_THREADSAFE );
	if( ret < 0 ) { printf( "----------------------------\n" ); } 

	printf( "bev addr [%x]\n", (int)bev );
	/* bufferevent_setcb(bev, NULL, conn_writecb, conn_eventcb, NULL); */
	// 在回调函数中使用user_data共享通信 什么情况释放, 由谁释放? 在bufferevent_free后释放
	char *userData = NULL;
	bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, userData );

	// 延迟设置 写调用
	//bufferevent_enable(bev, EV_WRITE);
	bufferevent_disable(bev, EV_WRITE);
	
	bufferevent_enable(bev, EV_READ);
	//bufferevent_disable(bev, EV_READ);

	/*
	printf( "will call bufferevent_write\n" );
	bufferevent_write(bev, MESSAGE, strlen(MESSAGE));
	printf( "end listener_cb \n" );
	*/
}

void * testThreadFun(void * bev )
{
	bufferevent_lock(bev);
	printf( "10 s sleep  at testThreadFun\n" );
	sleep(10);
	printf( "up at testThreadFun\n" );
	bufferevent_unlock(bev);
}
/*
 * bufferevent_read 不带结束符
 * data[len]='\0';
 * */
static void
conn_readcb(struct bufferevent *bev, void *user_data)
{
	printf( "begin conn_readcb\n" );
	printf( "bev addr [%x]\n", (int)bev );

	struct evbuffer *input = bufferevent_get_input(bev);
	pthread_t testThread, testThread2;

	//test lock
	int ret = pthread_create(&testThread, NULL, testThreadFun, bev);
	if( -1 == ret ) { printf( "create thread error\n" ); } 

	ret = pthread_create(&testThread2, NULL, testThreadFun, bev);
	if( -1 == ret ) { printf( "create thread error\n" ); } 

	char data[100];
	int len = 0;

	dPrt( "input len [%d]\n", evbuffer_get_length(input) ) ;
	len = bufferevent_read(bev, data, sizeof(data)-1 ); 
	//data[len]='\0';
	dPrt( "data[%d][%d][%s]\n", len,strlen(data), data );

	dPrt( "input len [%d]\n", evbuffer_get_length(input) ) ;
	/* 
	struct evbuffer *input = bufferevent_get_input(bev);
	if (evbuffer_get_length(input) == 0) {
		printf("flushed answer\n");
		bufferevent_free(bev);
	}
	*/

	bufferevent_enable(bev, EV_WRITE);
	printf( "will call bufferevent_write\n" );
	bufferevent_write(bev, MESSAGE, strlen(MESSAGE));
	//flag = 1;
	bufferevent_write(bev, MESSAGE, strlen(MESSAGE));
	printf( "end listener_cb \n" );

	printf( "end conn_readcb\n" );
}

static void
conn_writecb(struct bufferevent *bev, void *user_data)
{
	printf( "begin conn_writecb \n" );
	printf( "bev addr [%x]\n", (int)bev );
	int len = 0;

	/*  关闭客户的连接
	*/
	struct evbuffer *output = bufferevent_get_output(bev);
	len = evbuffer_get_length(output);
	printf( "len[%d]\n", len );
	if ( 0 == len && flag /* evbuffer_get_length(output) == 0 && flag*/ ) {
		printf("flushed answer\n");
		bufferevent_free(bev);
	}

	printf( "end conn_writecb \n" );
}

static void
conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
	printf( "begin conn_eventcb\n" );
	if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");
	} else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on the connection: %s\n",
		    strerror(errno));/*XXX win32*/
	}
	/* None of the other events can happen here, since we haven't enabled
	 * timeouts */
	bufferevent_free(bev);
	printf( "end conn_eventcb %d\n", events );
}

static void
signal_cb(evutil_socket_t sig, short events, void *user_data)
{
	struct event_base *base = user_data;
	struct timeval delay = { 2, 0 };

	printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");

	event_base_loopexit(base, &delay);
}
