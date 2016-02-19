#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <string.h>

class cSplit
{
    public:
        cSplit()
    {
    }

        void DoSplit(const std::string& sSrc, const std::string& splitor, std::vector<std::string>& vecTokens)
        {
            vecTokens.clear();

            if (sSrc.empty() || splitor.empty())
            {
                return;
            }

            std::string::size_type start = 0;
            while (start < sSrc.size())
            {
                std::string::size_type pos = 0;
                pos = sSrc.find(splitor, start);
                if (pos == std::string::npos)
                {
                    vecTokens.push_back(sSrc.substr(start));
                    break;
                }

                std::string stmp = sSrc.substr(start, pos-start);
                if (!stmp.empty())
                {
                    vecTokens.push_back(stmp);
                }

                start = pos + splitor.size();
            }
        }

        void DoSplit(const std::string& sSrc, const char splitor, std::vector<std::string>& vecTokens)
        {
            vecTokens.clear();

            std::stringstream stream(sSrc);
            std::string stoke;
            while (std::getline(stream, stoke, splitor))
            {
                if (!stoke.empty())
                {
                    vecTokens.push_back(stoke);
                }
            }
        }

//use strtok,if  the seperator  is a string, every char in string is a seperator
//make a mistack think the whole string is a seperator.
        void DoSplit(char* src, char* seprator, std::vector<std::string>& vecTokens)
        {
            vecTokens.clear();

            if (src==NULL || seprator==NULL)
            {
                return;
            }

            char* token;
        
            token = strtok(src, seprator);
            while (token != NULL)
            {
                std::string s(token);
                if (!s.empty())
                {
                    vecTokens.push_back(s);
                }
        
                token = strtok(NULL, seprator);
            }
        }
};

class cTest
{
    public:
        void DoRunTest1()
        {
            std::cout <<"DoRunTest1\n";
            std::string sSrc("hello world  x       ");
            char c = ' ';
            cSplit test1;
            test1.DoSplit(sSrc, c, m_vecTokens);
        }

        void DoRunTest2()
        {
            std::cout << "DoRunTest2\n";
            char src[] = "hello world x            ";
            char seprator[] = "ll";
            cSplit test;
            test.DoSplit(src, seprator, m_vecTokens);
        }

        void DoRunTest3()
        {
            std::cout << "DoRunTest3\n";
            std::string sSrc("llhello world x            ");
            std::string seprator("ll");
            cSplit test;
            test.DoSplit(sSrc, seprator, m_vecTokens);
        }

        void Display()
        {
            std::cout <<"vec:\n";
            for (int i=0; i<m_vecTokens.size(); i++)
            {
                std::cout << m_vecTokens[i] << std::endl; 
            }
        }

    private:
        std::vector<std::string> m_vecTokens;
};

int main()
{
    cTest test;
    test.DoRunTest3();
    test.Display();

    test.DoRunTest1();
    test.Display();

    test.DoRunTest2();
    test.Display();
    return 0;
}
