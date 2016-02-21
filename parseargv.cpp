#include <unistd.h>
#include <sstream>
#include <iostream>
#include <stdio.h>

void Usage()
{
    std::cout << " hH?vVa:b:c::\n";
} 

void Version()
{
    std::cout << "verson:ver1.0\n";
}

template <class TI, class TO>
TO ToType(const TI& a)
{
    std::stringstream ss;
    ss << a;
    TO b;
    ss >> b;

    return b; 
}

bool InitArgv(int argc, char** argv)
{
    int num = 3;
    char format[] = "hH?vVa:b:c:";
    /*
    if (argc >  3)
    {
        std::cout <<"argc num wrong\n";
        Usage();
        return false;
    }
    */

    char c;
    std::string s;
    int b;
    while ((c=getopt(argc, argv, format)) != EOF)
    {
        switch (c)
        {
            case  'h':
            case 'H':
            case '?':
                Usage();
                break;

            case 'v':
            case 'V':
                Version();
                break;

            case 'a':
                s = optarg;
                std::cout <<"opt a:" << s <<std::endl;
                break;

            case 'b':
                b = ToType<char*, int>(optarg);
                std::cout <<"opt b:" << b<< std::endl;
                break;

            case 'c':
                s=optarg;
                std::cout <<"opt c:"<< s<< std::endl;
                break;

            default:
                Usage();
        }
    }
}

int main(int argc, char** argv)
{
    
    InitArgv(argc, argv);   
    return 0;
}
