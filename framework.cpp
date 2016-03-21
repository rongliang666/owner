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

class  cPackageByHead
{
    enum 
    {
            PKG_HEAD_MARK=0x02,
            PKG_TAIL_MARK=0x03,
    };

    enum 
    {
                MAX_PKG_SIZE=(unsigned short)0xffff,
    };

    public:
    cPackageByHead(){}
    ~cPackageByHead(){}

    static inline int HasWholePkg(const char* pszData, const int iDataSize, int& iRealPkgLen, int& iPkgLen)
    {
        int iRe = -2;
        if (iDataSize <= 0)
        {
            return -1;
        }

        if (iDataSize <= 3)
        {
            iRe = -1;
            if ( *pszData != PKG_HEAD_MARK )
            {
                iRe = -2;
            }
        }
        else
        {
            iPkgLen = ntohs(*((unsigned short*)(pszData+1)));
            {
                if ( iDataSize >= iPkgLen && iPkgLen >= 4  )
                {
                    if ( *(pszData+iPkgLen-1) == PKG_TAIL_MARK )
                    {
                        iRealPkgLen = iPkgLen - 4;
                        iRe = 0;
                    }
                    else
                    {
                        iRe = -2;
                    }
                }
                else
                {
                    iRe = -1;
                    if ( *pszData != PKG_HEAD_MARK || iPkgLen < 4 )
                    {
                        iRe = -2;
                    }
                }
            }
        }
       
       return iRe; 
    }

    static inline const char* MakeSendPkg(std::string& sSendBuf, const char* pszData, const int iDataSize, int& iSendDataSize)
    {
        if ( iDataSize > MAX_PKG_SIZE-4 || iDataSize <= 0 )
        {
            return NULL;
        }

        sSendBuf.resize(iDataSize+4);
        sSendBuf[0] = PKG_HEAD_MARK;
        sSendBuf[iDataSize+3] = PKG_TAIL_MARK;
        *((unsigned short*)(sSendBuf.data()+1)) = htons(iDataSize+4);
        memcpy((char*)sSendBuf.data()+3, pszData, iDataSize);
        iSendDataSize = (int)sSendBuf.size();
        return sSendBuf.data();
    }

    inline static const char* GetRealPkgData(const char* pszData, const size_t nDataSize)
    {
        return pszData+3;
    }
};

class cTcpConn
{
    public:
        cTcpConn()
        {
            memset(&m_sin, 0, sizeof(m_sin));
            m_ptrBase  = NULL;
            m_ptrListener = NULL;
            m_ptrInFifo = NULL;
            m_ptrOutFifo = NULL;
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
            if (m_ptrInFifo != NULL)
            {
                delete m_ptrInFifo;
                m_ptrInFifo = NULL;
            }

            if (m_ptrOutFifo != NULL)
            {
                delete m_ptrOutFifo;
                m_ptrOutFifo = NULL;
            }
        }

        bool Init(std::string& sIp, uint16_t iPort, struct event_base* ptr, int iInSize, int iOutSize)
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

            m_ptrInFifo = new  CFIFOBuffer;
            if (m_ptrInFifo == NULL)
            {
                cout <<"new CFIFOBuffer failed.\n";
                return false;
            }
            m_ptrInFifo->Init(iInSize);

            m_ptrOutFifo = new CFIFOBuffer;
            if (m_ptrOutFifo == NULL)
            {
                cout <<"new CFIFOBuffer failed.\n";
                return false;
            }
            m_ptrOutFifo->Init(iOutSize);

            evconnlistener_set_error_cb(m_ptrListener, AcceptErrCB);
        }

    private:
            
            static void echo_read_cb(struct bufferevent *bev, void *ctx)
            {
                std::cout <<"echo_read_cb\n";
                char buf[1];
                char sbuf[1024*1024];
                memset(sbuf, 0, sizeof(sbuf));
                int count = 0;
                int pos = 0;
            
                while(1)
                {

                struct evbuffer *input = bufferevent_get_input(bev);
                int n = bufferevent_read(bev, buf, sizeof(buf));
                if (n <= 0)
                {
                    std::cout <<"bufferevent_read error :" << n << std::endl;
                    return;
                }
                else
                {
                    int32_t iWholeFlag = 0;
                    int32_t nPkgLen = 0;
                    int32_t nRealPkgLen = 0;
                    memcpy(sbuf+pos, buf, n);
                    pos += n;
                    std::cout <<"count is " <<count << std::endl;
                    count++;
                    if (iWholeFlag=cPackageByHead::HasWholePkg(sbuf, pos, nRealPkgLen, nPkgLen) == 0)
                    {
                        const char* pstRealPkgData = cPackageByHead::GetRealPkgData(sbuf, sizeof(sbuf));
                        std::string s(pstRealPkgData, nRealPkgLen);
                        //m_ptrInFifo->Write(s);
                        std::string sout;
                        //m_ptrInFifo->Read(sout);
                        std::cout <<"real pkg:" << sout << std::endl;
                        memset(sbuf, 0, sizeof(sbuf));
                        pos = 0;
                    }

                }
                }

                //sleep(10000);
            
                //struct evbuffer *output = bufferevent_get_output(bev);
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
        CFIFOBuffer* m_ptrInFifo;
        CFIFOBuffer* m_ptrOutFifo;
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

        bool Init(std::string sLogPath)
        {
            msglog.Init(sLogPath, 1024*1024*10, 5, false);

            m_ptrBase = event_base_new();
            if (!m_ptrBase)
            {
                cout << "event_base_new failed\n";
                return false;
            }
            
            const char* ptr = event_base_get_method(m_ptrBase);
            msglog.Write("event_base_get_method get base use(%s)", ptr);
            return true;
        }

        bool InitConn(std::string& sIp, uint16_t iPort, int iInSize, int iOutSize)
        {
            conn.Init(sIp, iPort, m_ptrBase, iInSize, iOutSize);
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

            while (event_base_dispatch(m_ptrBase) == 0)
            {
                event_base_dispatch(m_ptrBase);
            }

            return true;
        }

    private:
        cTcpConn conn;
        cTimer timer;
        cSignal signal;
        struct event_base* m_ptrBase;
        CFileLog msglog; 
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
    std::string sTaskLogPath = sLogPath.data() ;
    sTaskLogPath += "/";
    sTaskLogPath += argv[0];
    sTaskLogPath += "_taskmsg";
    task.Init(sTaskLogPath);
    task.InitConn(sIp, iport, 1024*1024*100, 1024*1024*100);
    task.InitTimer();
    task.InitSignal();

    std::string name = "timer1";
    struct timeval tval = {1,0};
    task.SetTimer(name, tval, Test);
    /*
    name = "timer2";
    tval.tv_sec = 5;
    tval.tv_usec = 0;
    task.SetTimer(name, tval, Test2);

    */
    task.SetSignal(SIGINT, SignalTest);

    task.RunDispatch();
    
    return 0;
}

