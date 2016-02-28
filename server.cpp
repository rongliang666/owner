#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include <assert.h>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>

#define MAX_LINE 16384

void ReadCB(struct bufferevent* bev, void* ctx)
{
    struct evbuffer* input;
    struct evbuffer*  output;
    char* line;
    size_t n;
    input = bufferevent_get_input(bev);
    output = bufferevent_get_input(bev);

    while((line=evbuffer_readln(input, &n, EVBUFFER_EOL_LF)))
    {
        std::cout <<"read:" << line<< std::endl;
        //evbuffer_add(output, line, n);
        //evbuffer_add(output, "\n", 1);
        free(line);
        break;
    }
}

void ErrCB(struct bufferevent* bev, short error, void* ctx)
{
    if (error & BEV_EVENT_EOF)
    {
        std::cout <<"connect has close\n";
        return;
    }
     else if (error & BEV_EVENT_ERROR)
     {
         std::cout <<"connect have error\n";
         return ;
     }

     bufferevent_free(bev);
    std::cout <<"ErrCB \n";
    std::cout <<"error: " << error <<"\tstring:" <<strerror(error) << std::endl;

}

void DoAccept(evutil_socket_t listener, short event, void* arg)
{
    struct event_base* base = (struct event_base*)arg;
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);

    int fd = accept(listener, (struct sockaddr*)&ss, &slen);
    if (fd < 0)
    {
        std::cout <<"accept failed\n";
        return;
    }

    struct bufferevent* bev;
    evutil_make_socket_nonblocking(fd);
    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, ReadCB, NULL, ErrCB, NULL);
    bufferevent_setwatermark(bev, EV_READ, 0, MAX_LINE);
    bufferevent_enable(bev, EV_READ|EV_WRITE);
}

void run()
{
    evutil_socket_t listener;
    struct sockaddr_in sin;
    struct event_base* base;
    struct  event* listen_event;

    base = event_base_new();
    if (base == NULL)
    {
        std::cout <<"event_base_new failed.\n";
        return ;
    }
    
    sin.sin_family =AF_INET;
    sin.sin_addr.s_addr  = 0;
    sin.sin_port = htons(20000);

    listener = socket(AF_INET, SOCK_STREAM, 0);
    evutil_make_socket_nonblocking(listener);
    if (bind(listener, (struct sockaddr*)&sin, sizeof(sin))< 0)
    {
        std::cout <<"bind failed.\n";
        return ;
    }

    if (listen(listener, 16) < 0)
    {
        std::cout <<"listen failed.\n";
        return ;
    }
    listen_event = event_new(base, listener, EV_READ| EV_PERSIST, DoAccept, (void*)base);
    if (listen_event == NULL)
    {
        std::cout <<"event_new failed\n";
        return ;
    }

    event_add(listen_event, NULL);
    event_base_dispatch(base);



}

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    run();

    return 0;
}

