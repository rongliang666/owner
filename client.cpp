#include <iostream>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>  

#include <boost/thread/thread.hpp>
#include "boost/function.hpp"

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

#define  PORT 20000
 #define  MAXDATASIZE 100
void Func(int i)
{
    int  sockfd, num;  
    char  buf[MAXDATASIZE];  
    struct hostent *he;  
    struct sockaddr_in server;  
    if((sockfd=socket(AF_INET, SOCK_STREAM, 0))==-1){  
    printf("socket()error\n");  
    return ;
    }

    bzero(&server,sizeof(server));  
    server.sin_family= AF_INET;  
    server.sin_port = htons(PORT);  
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    if(connect(sockfd,(struct sockaddr *)&server,sizeof(server))==-1){ 
    printf("connect()error\n");  
    return ;
    }

    char str[100] = "hello world" ;
    snprintf(str, sizeof(str), "hello world %d\n", i);
    std::string s;
    int len;
    cPackageByHead::MakeSendPkg(s, str, strlen(str), len);
    if (send(sockfd, s.c_str(), s.size(), 0) < 0)
    {
        std::cout <<"send failed\n";
    }

    sleep(10);
    close(sockfd);

}

int main(void)
{
    boost::thread t1(boost::bind(&Func, 1));
    boost::thread t2(boost::bind(&Func, 2));
    boost::thread t3(boost::bind(&Func, 3));
    boost::thread t4(boost::bind(&Func, 4));
    boost::thread t5(boost::bind(&Func, 5));
    boost::thread t6(boost::bind(&Func, 6));
    boost::thread t7(boost::bind(&Func, 7));
    boost::thread t8(boost::bind(&Func, 8));
    boost::thread t9(boost::bind(&Func, 9));

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    t9.join();

}

