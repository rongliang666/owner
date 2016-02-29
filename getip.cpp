#include <iostream>
#include <string.h>
#include <sys/types.h> 
#include <netinet/in.h>
#include <sys/socket.h> 
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <errno.h>

int GetLocalIp(std::string& sIp, std::string sName)
{
    int fd;
    fd =socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        return -1;
    }

    struct ifreq req;
    memset(&req, 0, sizeof(req));
    strncpy(req.ifr_name, sName.c_str(), sName.size());
    if (ioctl(fd, SIOCGIFADDR, &req) < 0)
    {
        std::cout <<"error: " <<errno << "\terrmsg:" <<strerror(errno) << std::endl;

        close(fd);
        return -1;
    }

    sIp = inet_ntoa(((struct sockaddr_in*)&req.ifr_addr)->sin_addr);

    close(fd);

    return 0;
}


int main()
{
    std::string sIp;
    std::string sName= "eth1";
    int ret = GetLocalIp(sIp, sName);
    if (ret != 0 )
    {
        std::cout << "GetLocalIp failed.\n";
        return -1;
    }

    std::cout << sIp << std::endl;


}
