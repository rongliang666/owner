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
        {}

        bool Init()
        {
            m_ptrBase = event_base_new();
            if (!m_ptrBase)
            {
                cout << "event_base_new failed\n";
                return 0;
            }

            m_ptrListener = evconnlistener_new_bind(m_ptrBase, (void(*)(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr *address, int socklen, void* ctx))&cTcpConn::AcceptCB, NULL, LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1, (struct sockaddr*)&m_sin, sizeof(m_sin));
            if (!m_ptrListener)
            {
                cout <<"evconnlistener_new_bind failed.\n";
                return 0;
            }
        }

        bool RunDispatch()
        {
            evconnlistener_set_error_cb(m_ptrListener, AcceptErrCB);
            event_base_dispatch(m_ptrBase);

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
    conn.RunDispatch();
    
    return 0;
}

