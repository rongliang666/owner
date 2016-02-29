#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

void echo_read_cb(struct bufferevent *bev, void *ctx)
{
    struct evbuffer *input = bufferevent_get_input(bev);
    struct evbuffer *output = bufferevent_get_output(bev);
    evbuffer_add_buffer(output, input);
}

void echo_event_cb(struct bufferevent *bev, short events, void *ctx)
{
    if (events & BEV_EVENT_ERROR)
    {
        cout <<"Error from bufferevent" << endl;
    }
    else if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR))
    {
        bufferevent_free(bev);
    }
}
void AcceptCB(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr *address, int socklen, void* ctx)
{
    struct event_base *base = evconnlistener_get_base(listener);
    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, echo_read_cb, NULL, echo_event_cb, NULL);
    bufferevent_enable(bev, EV_READ|EV_WRITE);
}

void AcceptErrCB(struct evconnlistener *listener, void* ctx) 
{
    struct event_base *base = evconnlistener_get_base(listener);
     int err = EVUTIL_SOCKET_ERROR();
     cout <<"Got an error:" << err << "on the listener. errmsg:"  << evutil_socket_error_to_string(err) << endl;
     event_base_loopexit(base, NULL);
}

int main()
{
    struct event_base* base;
    struct evconnlistener* listener;
    struct sockaddr_in sin;

    int port = 20000;

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(0);
    sin.sin_port =htons(port);

    base = event_base_new();
    if (!base)
    {
        cout << "event_base_new failed\n";
        return 0;
    }

    listener = evconnlistener_new_bind(base, AcceptCB, NULL, LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1, (struct sockaddr*)&sin, sizeof(sin));
    if (!listener)
    {
        cout <<"evconnlistener_new_bind failed.\n";
        return 0;
    }

    evconnlistener_set_error_cb(listener, AcceptErrCB);
    event_base_dispatch(base);

    return 0;
}
