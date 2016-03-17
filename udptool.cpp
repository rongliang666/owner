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

inline uint64_t diff_msec(timeval stStart,timeval stEnd)
{
        return (stEnd.tv_sec-stStart.tv_sec)*1000 + stEnd.tv_usec/1000 - stStart.tv_usec/1000;
}
/*
 *  * @param [in] szReqBuf, 待发送的包体缓冲区
 *  * @param [in] iReqLen, 待发送的包体长度
 *  * @param [in] ulSendPerSec, 发送速度，单位s
 *  * @param [in] ulSvrPort, 服务器端口
 *  * @param [in] ulSvrIp, 服务器ip
 *  * @param [in] ulSvrPortSize, 服务器开启的端口数
 *  * @param [in] iForkNum, 发包工具发包速率达不到测试要求的时候，可以启动此参数
 *  */
int MultiSend(char *szReqBuf,int iReqLen,uint32_t ulSendPerSec,uint16_t usSvrPort,string strSvrIp,uint32_t ulSvrPortSize,int iForkNum = 0)
{
    /*****************创建多个进程*******************/
    int iForkPid = 0;
    for (int i = 0; i < iForkNum; ++i)
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
   int iSocket = 0;
   if ((iSocket=socket(AF_INET, SOCK_DGRAM, 0)) == -1)
   {
       return -1;
   }

   int iFlags = fcntl(iSocket, F_GETFL, 0);
   iFlags |= O_NONBLOCK;//设置非阻塞
   iFlags |= O_NDELAY;
   fcntl(iSocket, F_SETFL, iFlags);

   struct sockaddr_in si_other;
   memset((char *) &si_other, 0, sizeof(si_other));
   si_other.sin_family = AF_INET;
   si_other.sin_port = htons(usSvrPort);
   si_other.sin_addr.s_addr = inet_addr(strSvrIp.c_str());
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
       if (ulDiff*ulSendPerSec/1000 < ulSendCnt)
       {
            usleep(10);
            continue;
       }
       int iPortBase = ulSvrPortSize;
       iPortBase = ulIndex++%ulSvrPortSize;
       uint16_t usTmpPort = usSvrPort + iPortBase*10;
       si_other.sin_port = htons(usTmpPort);
       sendto(iSocket,szReqBuf,iReqLen,0,(struct sockaddr*)&si_other,iLen);
       ulSendCnt++;
       recvfrom(iSocket,szRecvBuf,iRecvLen,0,(struct sockaddr *)&si_other,(socklen_t*)&iSockLenRecv);
   }

   return 0;
}

template<class TI, class TO>
TO ToType(TI a)
{
    stringstream s;
    s << a;
    TO b;
    s >> b;
    return b;    
}

int main(int argc, char** argv)
{
    string ip="172.27.10.73";
    int port = 28813;
    char szReqBuf[2] = "h";
    if (argc != 2)
    {
        cout <<"./tool num\n";
        return 0;
    }
    int count = ToType<string, int>(argv[1]);
    MultiSend(szReqBuf, sizeof(szReqBuf), count, port, ip, 1);       

    return 0;
}

