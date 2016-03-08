#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <stdlib.h>
#include "comm_tool.h"

using namespace std;

class cTcpConn
{
    public:
        cTcpConn(string& sIp, uint16_t iPort):m_sIp(sIp), m_iPort(iPort)
        {
            memset(&m_sin, 0, sizeof(m_sin));
            m_sin.sin_family = AF_INET;
            m_sin.sin_addr.s_addr = inet_addr(sIp.c_str());
            m_sin.sin_port =htons(iPort);
        }

        ~cTcpConn()
        {
            if (m_ptrListener != NULL)
            {
                evconnlistener_free(m_ptrListener);
                m_ptrListener = NULL;
            }

            if (m_ptrBase != NULL)
            {
                event_base_free(m_ptrBase);
                m_ptrBase = NULL;
            }
        }

        bool InitTcp()
        {
            if (m_ptrBase == NULL)
            {
                return false;
            }

            m_ptrListener = evconnlistener_new_bind(m_ptrBase, (void(*)(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr *address, int socklen, void* ctx))&cTcpConn::AcceptCB, NULL, LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1, (struct sockaddr*)&m_sin, sizeof(m_sin));
            if (!m_ptrListener)
            {
                cout <<"evconnlistener_new_bind failed.\n";
                return 0;
            }

        }

        bool InitTimer()
        {
            if (m_ptrBase == NULL)
            {
                return false;
            }

            struct timeval tv = {1, 0};
            struct event* ev = NULL;
            ev = event_new(m_ptrBase, -1, EV_PERSIST, TimeCallBack, ev);
            if (ev == NULL)
            {
                return false;
            }

            evtimer_add(ev, &tv);
        }

        bool InitSignal()
        {
            if (m_ptrBase == NULL)
            {
                return false;
            }
        }

        bool Init()
        {
            m_ptrBase = event_base_new();
            if (!m_ptrBase)
            {
                cout << "event_base_new failed\n";
                return 0;
            }

            InitTcp();
            InitTimer();
            InitSignal();
        }

        bool RunDispatch()
        {
            evconnlistener_set_error_cb(m_ptrListener, AcceptErrCB);
            event_base_dispatch(m_ptrBase);

        }
    private:
        static void TimeCallBack(evutil_socket_t fd, short event, void* opt)
        {
            struct timeval tv = {1, 0};
            //evtimer_add((struct event*)(opt), &tv);
            std::cout<<"TimeCallBack func\n";
        }

            static void echo_read_cb(struct bufferevent *bev, void *ctx)
            {
                std::cout <<"echo_read_cb\n";
                char buf[1000];
            
                struct evbuffer *input = bufferevent_get_input(bev);
                int n = bufferevent_read(bev, buf, sizeof(buf));
                if (n <= 0)
                {
                    std::cout <<"bufferevent_read error :" << n << std::endl;
                    return;
                }
                else
                {
                    buf[n] = '\0';
                    std::cout <<"bufferevent_read  msg: " << buf << std::endl;
                }
            
                struct evbuffer *output = bufferevent_get_output(bev);
            }
            
            static void echo_event_cb(struct bufferevent *bev, short events, void *ctx)
            {
                std::cout <<"echo_event_cb\n";
                if (events & BEV_EVENT_ERROR)
                {
                    cout <<"Error from bufferevent" << endl;
                }
                else if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR))
                {
                    cout <<"event accept close event\n";
                    bufferevent_free(bev);
                }
            }
            static void AcceptCB(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr *address, int socklen, void* ctx)
            {
                std::cout <<"AcceptCB\n";
                struct event_base *base = evconnlistener_get_base(listener);
                struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
                bufferevent_setcb(bev, echo_read_cb, NULL, echo_event_cb, NULL);
                bufferevent_enable(bev, EV_READ|EV_WRITE);
            }
            
            static void AcceptErrCB(struct evconnlistener *listener, void* ctx) 
            {
                std::cout <<"AcceptErrCB\n";
                struct event_base *base = evconnlistener_get_base(listener);
                 int err = EVUTIL_SOCKET_ERROR();
                 cout <<"Got an error:" << err << "on the listener. errmsg:"  << evutil_socket_error_to_string(err) << endl;
                 event_base_loopexit(base, NULL);
            }

        private:
        struct sockaddr_in m_sin;
        string m_sIp;
        uint16_t m_iPort;
        struct event_base* m_ptrBase;
        struct evconnlistener* m_ptrListener;
};

class cTimer
{
    public:
        cTimer()
        {
            m_ptrBase = NULL;
        }

        ~cTimer()
        {}

        bool Init()
        {
            m_ptrBase = event_base_new();
            if (!m_ptrBase)
            {
                cout << "event_base_new failed\n";
                return false;
            }

            return true;
        }

        bool SetTimer(std::string sName, struct timeval tv)
        {
            st_Timer t;
            t.name = sName;
            t.tv = tv;
            m_mapTimers[sName] = t;

            if (m_ptrBase  == NULL)
            {
                std::cout <<"m_ptrBase NULL\n";
                return false;
            }

            struct event* ev = NULL;
            ev = event_new(m_ptrBase, -1, EV_PERSIST, TimeCallBack, &t);
            if (ev == NULL)
            {
                return false;
            }

            m_mapEvents[sName] = ev;
            event_add(ev, &t.tv);
        }

        bool RunDispatch(void)
        {
            event_base_dispatch(m_ptrBase);
        }

        typedef struct stTimer
        {
            std::string name;
            struct timeval tv;
        }st_Timer;

    private:
        static void TimeCallBack(evutil_socket_t fd, short event, void* opt)
        {
            st_Timer* ptr = (st_Timer*)opt;
            //evtimer_add((struct event*)(opt), &tv);
            std::cout<<"time name:" << ptr->name << "\tin TimeCallBack func\n";
        }

    private:
        struct event_base* m_ptrBase;
        std::map<std::string, st_Timer> m_mapTimers;
        std::map<std::string, struct event*> m_mapEvents;

};

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout <<"need too argv\n";
        return 0;
    }

    CFileLog msgLog;
    CConfig oCfg;
    if (!oCfg.LoadConfig(argv[1]))
    {
        std::cout <<"load config failed," << argv[1] << std::endl;
        return 0;
    }

    CValue iport = oCfg.GetValue("tcp", "port", 20000);
    CValue sLogPath = oCfg.GetValue("comm", "log", "./");
    msgLog.Init((std::string)sLogPath+"/"+argv[0]+"_msg", 1024*1024*100, 5, false);
    msgLog.Write("in server_ver2");
    msgLog.Write("port: %d" ,(uint16_t)iport) ;
    msgLog.Write("sLogPath :%s", ((std::string)sLogPath).c_str());

    std::string sIp = "127.0.0.1";

    cTcpConn conn(sIp, iport);
    conn.Init();

    std::cout <<"after cTcpConn\n";
    cTimer t;
    std::string name = "timer1";
    struct timeval tval = {1,0};
    t.Init();
    t.SetTimer(name, tval);
    conn.RunDispatch();
    t.RunDispatch();
    
    return 0;
}

