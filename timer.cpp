#include <iostream>
#include <event2/event.h>

static int count = 0;
void cb_timerfunc(evutil_socket_t fd, short what, void* arg)
{
    struct event_base* base = (event_base*)arg;
    std::cout << "int cb_timerfunc.cout is " << count++ << std::endl;
    if (count > 10)
    {
        //event_del(me);
        event_base_loopbreak(base);
    }
}
int main(void)
{
    struct event_base* base = event_base_new();
    if (!base)
    {
        std::cout << "event_base_new failed\n";
    }

    struct event* timer = event_new(base, -1, EV_PERSIST, cb_timerfunc, base);
    struct timeval one_sec ={1, 0};
    event_add(timer, &one_sec);
    event_base_dispatch(base);

}
