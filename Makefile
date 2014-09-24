all:
	gcc -I /opt/libevent_sample/libevent-2.0.20-stable ../evthread_pthread.c regress.c regress_minheap.c regress_et.c regress_main.c regress_buffer.c regress_testutils.c regress_rpc.c regress_bufferevent.c tinytest.c regress.gen.c regress_util.c  regress_dns.c regress_http.c regress_thread.c regress_listener.c -levent -lpthread
