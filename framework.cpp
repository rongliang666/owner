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

typedef void (*TimerFunc)();
typedef void (*SignalFunc)();

class cTcpConn
{
    public:
        cTcpConn()
        {
            memset(&m_sin, 0, sizeof(m_sin));
            m_ptrBase  = NULL;
            m_ptrListener = NULL;
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

        bool Init(std::string& sIp, uint16_t iPort, struct event_base* ptr)
        {
            m_sIp = sIp;
            m_iPort = iPort;

            m_sin.sin_family = AF_INET;
            m_sin.sin_addr.s_addr = inet_addr(sIp.c_str());
            m_sin.sin_port  = htons(iPort);
            m_ptrBase = ptr;

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

            evconnlistener_set_error_cb(m_ptrListener, AcceptErrCB);
        }

    private:
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
            m_mapTimers.clear();
            m_mapEvents.clear();
        }

        ~cTimer()
        {
            std::map<std::string, struct event*>::iterator iter;
            for (iter=m_mapEvents.begin(); iter!=m_mapEvents.end(); iter++)
            {
                if (iter->second != NULL)
                {
                    event_free(iter->second);
                    iter->second = NULL;
                }
            }

            std::map<std::string, st_Timer*>::iterator iterTimer;
            for (iterTimer=m_mapTimers.begin(); iterTimer!=m_mapTimers.end(); iterTimer++)
            {
                if (iterTimer->second != NULL)
                {
                    delete iterTimer->second;
                    iterTimer->second = NULL;
                }
            }
        }

        bool Init(struct event_base* ptr)
        {
            m_ptrBase = ptr;
            if (m_ptrBase == NULL)
            {
                cout << "event_base_new failed\n";
                return false;
            }

            return true;
        }

        bool SetTimer(std::string sName, struct timeval tv, TimerFunc func)
        {
            st_Timer* t = new st_Timer ;
            t->name = sName;
            t->tv = tv;
            t->func = func;

            if (m_ptrBase  == NULL)
            {
                std::cout <<"m_ptrBase NULL\n";
                return false;
            }

            struct event* ev = NULL;
            ev = event_new(m_ptrBase, -1, EV_PERSIST, TimeCallBack, t);
            if (ev == NULL)
            {
                return false;
            }

            m_mapTimers[sName] = t;
            m_mapEvents[sName] = ev;
            event_add(ev, &t->tv);
        }

        typedef struct stTimer
        {
            std::string name;
            struct timeval tv;
            TimerFunc func;
        }st_Timer;

    private:
        static void TimeCallBack(evutil_socket_t fd, short event, void* opt)
        {
            st_Timer* ptr = (st_Timer*)opt;
            std::cout<<"time name:" << ptr->name << "\tin TimeCallBack func\n";
            ptr->func();
        }

    private:
        struct event_base* m_ptrBase;
        std::map<std::string, st_Timer*> m_mapTimers;
        std::map<std::string, struct event*> m_mapEvents;
};

class cSignal
{
    public:
        cSignal()
        {
            m_ptrBase = NULL;
            m_mapSignal.clear();
            m_mapEvents.clear();
        }

        ~cSignal()
        {
            std::map<int, STSignal*>::iterator iter;
            for (iter=m_mapSignal.begin(); iter!=m_mapSignal.end(); iter++)
            {
                if (iter->second != NULL)
                {
                    delete iter->second;
                    iter->second = NULL;
                }
            }    

            std::map<int, struct event*>::iterator iterevent;
            for (iterevent=m_mapEvents.begin(); iterevent!=m_mapEvents.end(); iterevent++)
            {
                if (iterevent->second != NULL)
                {
                    event_free(iterevent->second);
                    iterevent->second = NULL;
                }
            }
        }

        bool Init(struct event_base* ptr)
        {
            m_ptrBase = ptr;
            if (ptr == NULL)
            {
                std::cout <<"Init failed. ptr is NULL\n";
                return false;
            }

            return true;
        }

        bool SetSignal(int signo, SignalFunc func)
        {
           STSignal* ptr = new STSignal;
           ptr->signal = signo;
           ptr->func = func;
            struct event* signal_int;
            signal_int = evsignal_new(m_ptrBase, signo, SignalCB, ptr);
            if (signal_int == NULL)
            {
                std::cout <<"SetSignal failed, evsignal_new return null\n";
                return false;
            }

            m_mapSignal[signo] = ptr;
            m_mapEvents[signo] = signal_int;
            event_add(signal_int, NULL);

            return true;
        }

        typedef struct stSignal
        {
            int signal;
            SignalFunc func;
        }STSignal;

    private:
        static void SignalCB(evutil_socket_t fd, short event, void *arg)
        {
            STSignal* ptr = (STSignal* )arg;
            std::cout <<"in SignalCB func. signalno:" << ptr->signal;
            ptr->func();
        }

    private:
        event_base* m_ptrBase;
        std::map<int, STSignal*> m_mapSignal; 
        std::map<int, struct event*> m_mapEvents;
};

class cTask
{
    public:
        cTask()
        {
            m_ptrBase = NULL;
        }

        virtual ~cTask()
        {
            if (m_ptrBase != NULL)
            {
                event_base_free(m_ptrBase);
            }
        }

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

        bool InitConn(std::string& sIp, uint16_t iPort)
        {
            conn.Init(sIp, iPort, m_ptrBase);
        }

        bool InitTimer()
        {
            timer.Init(m_ptrBase);
        }

        bool InitSignal()
        {
            signal.Init(m_ptrBase);
        }

        bool SetTimer(std::string& sName, struct timeval val, TimerFunc func)
        {
            timer.SetTimer(sName, val, func);
        }

        bool SetSignal(int sign, SignalFunc func)
        {
            signal.SetSignal(sign, func);
        }

        bool RunDispatch()
        {
            if (m_ptrBase == NULL)
            {
                std::cout << "RunDispatch failed. NULL\n";
                return false;
            }

            event_base_dispatch(m_ptrBase);
            return true;
        }

    private:
        cTcpConn conn;
        cTimer timer;
        cSignal signal;
        struct event_base* m_ptrBase;
};

void Test()
{
    std::cout <<"TimerFunc test\n";
}

void Test2()
{
    std::cout <<"TimerFunc Test2\n";
}

void SignalTest()
{
    std::cout <<"int SignalTest\n";
}

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

    cTask task;
    task.Init();
    task.InitConn(sIp, iport);
    task.InitTimer();
    task.InitSignal();

    std::string name = "timer1";
    struct timeval tval = {1,0};
    task.SetTimer(name, tval, Test);
    name = "timer2";
    tval.tv_sec = 5;
    tval.tv_usec = 0;
    task.SetTimer(name, tval, Test2);

    task.SetSignal(SIGINT, SignalTest);

    task.RunDispatch();
    
    return 0;
}

