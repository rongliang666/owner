#include <unistd.h>
#include <iostream>
#include <string.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>

using namespace std;


template<class TI, class TO>
TO ToType(TI a)
{
    stringstream s;
    s << a;
    TO b;
    s >> b;
    return b;    
}

class cUdpTool
{

    public:
        cUdpTool(std::string& sDstIp, uint32_t iDstPort, uint32_t iDstPortNum, uint32_t iCountPerSec, uint32_t iThreadNum=0)
            :m_sDstIp(sDstIp), m_iDstPort(iDstPort), m_iDstPortNum(iDstPortNum), m_iCountPerSec(iCountPerSec), m_iThreadNum(iThreadNum)
        {

        }

        ~cUdpTool()
        {

        }

        bool SetSendMsg(std::string& sMsg)
        {
            m_sSendMsg = sMsg;
        }

        bool Send()
        {
            /*****************创建多个进程*******************/
            int iForkPid = 0;
            for (int i = 0; i < m_iThreadNum; ++i)
            {
                iForkPid = fork();
                if (iForkPid == 0)
                {
                    break;
                }
            }
            //父进程休眠即可
            if (iForkPid > 0)
            {
                while(true)
                sleep(1);
            }
            /*****************创建多个进程*******************/

        ///*****************创建socket**********************  
            if ((m_iSockFd=socket(AF_INET, SOCK_DGRAM, 0)) == -1)
            {
                return false;
            }

            int iFlags = fcntl(m_iSockFd, F_GETFL, 0);
            iFlags |= O_NONBLOCK;//设置非阻塞
            iFlags |= O_NDELAY;
            fcntl(m_iSockFd, F_SETFL, iFlags);

            struct sockaddr_in si_other;
            memset((char *) &si_other, 0, sizeof(si_other));
            si_other.sin_family = AF_INET;
            si_other.sin_port = htons(m_iDstPort);
            si_other.sin_addr.s_addr = inet_addr(m_sDstIp.c_str());
            uint32_t iLen = sizeof(si_other);
            timeval stStart,stEnd;
            gettimeofday(&stEnd,NULL);
            stStart = stEnd;
            uint64_t ulSendCnt = 0;
            uint32_t ulIndex = 0;
            int iSockLenRecv = iLen;
            char szRecvBuf[1024];
            int iRecvLen = 1024;
            while (true)
            {
                gettimeofday(&stEnd,NULL);
                uint64_t ulDiff = diff_msec(stStart,stEnd);
                if (ulDiff*m_iCountPerSec/1000 < ulSendCnt)
                {
                     usleep(10);
                     continue;
                }
                uint32_t iPortBase = ulIndex++%m_iDstPortNum;
                uint16_t usTmpPort = m_iDstPort + iPortBase;
                si_other.sin_port = htons(usTmpPort);
                sendto(m_iSockFd,m_sSendMsg.c_str(),m_sSendMsg.size(),0,(struct sockaddr*)&si_other,iLen);
                ulSendCnt++;
                recvfrom(m_iSockFd,szRecvBuf,iRecvLen,0,(struct sockaddr *)&si_other,(socklen_t*)&iSockLenRecv);
            }
        }

    private:
        inline uint64_t diff_msec(timeval stStart,timeval stEnd)
        {
                return (stEnd.tv_sec-stStart.tv_sec)*1000 + stEnd.tv_usec/1000 - stStart.tv_usec/1000;
        }
        
    private:
        std::string m_sDstIp;//dst ip
        uint32_t m_iDstPort; //dst port
        uint32_t m_iCountPerSec; //send cout per second
        uint32_t m_iThreadNum; //mult thread to send. when send  not fast enough one thread
        uint32_t m_iDstPortNum; //dst port num DstPort+i....

        int m_iSockFd;
        std::string m_sSendMsg;
};

int main(int argc, char** argv)
{
    string ip="172.27.10.73";
    int port = 28813;
    char szReqBuf[2] = "h";
    int portsize = 1;
    int threadNum = 0;
    int count = 10000;

    if (argc < 3)
    {
        cout <<"./UdpTool ip port [portsize count threadnum]\n";
        return 0;
    }

    switch (argc)
    {
        case 3:
            ip = argv[1];
            port = ToType<string, int>(argv[2]);
            break;
        case 4:
            ip = argv[1];
            port = ToType<string, int>(argv[2]);
            portsize = ToType<string, int>(argv[3]);
            break;
        case 5:
            ip = argv[1];
            port = ToType<string, int>(argv[2]);
            portsize = ToType<string, int>(argv[3]);
            count = ToType<string, int>(argv[4]);
            break;
        case 6:
            ip = argv[1];
            port = ToType<string, int>(argv[2]);
            portsize = ToType<string, int>(argv[3]);
            count = ToType<string, int>(argv[4]);
            threadNum = ToType<string, int>(argv[5]);
            break;
    }

    std::string sMsg = "h";
    cUdpTool UdpTool(ip, port, portsize,count, threadNum);
    UdpTool.SetSendMsg(sMsg);
    UdpTool.Send();

    return 0;
}

