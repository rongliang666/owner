#include <iostream>
#include <errno.h>
#include <sstream>
#include <vector>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class cScan
{
    public:
        cScan(const char* sAddr, int port)
    {
        memset(&m_addr_serv, 0, sizeof(m_addr_serv));
        m_addr_serv.sin_family = AF_INET;
        m_addr_serv.sin_port = htons(port);
        m_addr_serv.sin_addr.s_addr = inet_addr(sAddr);
        m_bIsOpen = false;
    }

        ~cScan()
        {}

        int GetPort()
        {
            return ntohs(m_addr_serv.sin_port);
        }

        bool DoScan()
        {
            m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (m_sockfd < 0)
            {
                std::cout << "socket failed.\n";
                std::cout <<"errno:" << errno << "\terrmsg:" <<strerror(errno) << std::endl;
                return false;
            }

            if (connect(m_sockfd, (struct sockaddr*)&m_addr_serv, sizeof(struct sockaddr)) < 0)
            {
                m_bIsOpen = false;
                close(m_sockfd);
                return true;
            }

            close(m_sockfd);

            m_bIsOpen = true;
            return true;
        }

        bool GetIsOpen()
        {
            return m_bIsOpen;
        }

    private:
        int m_sockfd;
        struct sockaddr_in m_addr_serv;
        bool m_bIsOpen;
};

template<class TI, class TO>
TO ToType( TI src) 
{
    std::stringstream ss;

    ss << src;
    TO dst;
    ss >> dst;

    return dst;
}

template<class T>
void Swap(T& a, T& b)
{
    T tmp = a;
    a = b;
    b = tmp;
}

class cMain
{
    public:
        cMain()
        {}

        ~cMain()
        {
            for (int i=0; i<m_vecPScans.size(); i++)
            {
                if (m_vecPScans[i] != NULL)
                {
                    delete m_vecPScans[i];
                    m_vecPScans[i] = NULL;
                }
            }
        }

        void DoRun()
        {
            Menu();
            ParsePortList(); 
            MakeScans();
            ScanPorts();
            DisplayResult();
        }

    private:
        void Menu()
        {
            std::cout << "Port:such as(1-10,100) " << std::flush;
            std::getline(std::cin, m_sSrcList);
        }

        std::vector<std::string> Split(const std::string& str, const char delimiter)
        {
            std::vector<std::string> vecTokens;
            std::stringstream sstream(str);
            std::string sToken;

            while(std::getline(sstream, sToken, delimiter))
            {
                if (!sToken.empty())
                {
                    vecTokens.push_back(sToken);
                }
            }

            return vecTokens;
        }
       
        void ParsePortList()
        {
            std::vector<std::string> vecStrings = Split(m_sSrcList, ',');
            for (int i=0; i<vecStrings.size(); i++)
            {
                std::vector<std::string> vecRanges = Split(vecStrings[i], '-');
                int iPort = 0;
                if (vecRanges.size()==0 || vecRanges.size()==1)
                {
                    iPort = ToType<std::string, int>(vecStrings[i]);
                    m_vecSrcPorts.push_back(iPort);
                }
                else if (vecRanges.size() == 2)
                {
                    int iMin = ToType<std::string, int>(vecRanges[0]);
                    int iMax = ToType<std::string, int>(vecRanges[1]);
                    if (iMin > iMax)
                    {
                        Swap(iMin, iMax);
                    }

                    for (int i=iMin; i<=iMax; i++)
                    {
                        m_vecSrcPorts.push_back(i);
                    }
                }
            }
        }

        void MakeScans()
        {
            for (int i=0; i<m_vecSrcPorts.size(); i++)
            {
                cScan* ptr = new cScan("127.0.0.1", m_vecSrcPorts[i]);
                if (ptr != NULL)
                {
                    m_vecPScans.push_back(ptr);
                }
            }
        }

        void ScanPorts()
        {
            for (int i=0; i<m_vecPScans.size(); i++)
            {
                if (m_vecPScans[i] != NULL)
                {
                    if (!m_vecPScans[i]->DoScan())
                    {
                        std::cout << "m_vecPScans failed \n";
                        return ;
                    }

                    if (m_vecPScans[i]->GetIsOpen())
                    {
                        m_vecOpenPorts.push_back(m_vecPScans[i]->GetPort());
                    }
                    else
                    {
                        m_vecClosePorts.push_back(m_vecPScans[i]->GetPort());
                    }
                }
            }
        }

        void DisplayResult()
        {
            std::cout <<"All Port Num " << m_vecSrcPorts.size() << " :\n";
            for (int i=0; i<m_vecSrcPorts.size(); i++)
            {
                std::cout << m_vecSrcPorts[i] << '\t';
            }
            std::cout << std::endl;

            std::cout <<"Open Port Num " << m_vecOpenPorts.size() << " :\n";
            for (int i=0; i<m_vecOpenPorts.size(); i++)
            {
                std::cout << m_vecOpenPorts[i] << '\t';
            }
            std::cout << std::endl;

            std::cout << "Close Port Num " << m_vecClosePorts.size() << " :\n";
            for (int i=0; i<m_vecClosePorts.size(); i++)
            {
                std::cout << m_vecClosePorts[i] << '\t';
            }
            std::cout << std::endl;
        }

    private:
        std::string m_sSrcList;
        std::vector<cScan*> m_vecPScans;
        std::vector<int> m_vecSrcPorts;
        std::vector<int> m_vecOpenPorts;
        std::vector<int> m_vecClosePorts;
};

int main(void)
{
    cMain test;
    test.DoRun();

    return 0;
}

