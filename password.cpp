#include<iostream>
#include<unistd.h>
#include <string.h>
#include <string>
#include <termios.h>
#include <curses.h>

const char* ptrPasswd = "hello world";

void Func(void)
{
    char* ptr = getpass("Enter Password:");
    if (ptr == NULL)
    {
        std::cout<<"getpass failed.\n";
        return ;
    }

    if (strlen(ptrPasswd) != strlen(ptr))
    {
        std::cout <<"password wrong\n";
        return ;
    }

    if (strncmp(ptrPasswd, ptr, strlen(ptrPasswd)) == 0)
    {
        std::cout<<"passowrd right\n";
        return ;
    }
    else
    {
        std::cout <<"password wrong\n";
        return ;
    }
}

class cPasswd
{
    public:
        cPasswd()
        {
            //tcgetattr(STDIN_FILENO, &m_old);
            noecho(); 
        }

        void GetPasswd()
        {
            /*
            struct termios tnew = m_old;
            tnew.c_lflag &= ~(ECHO|ICANON);
            tcsetattr(STDIN_FILENO, TCSANOW, &tnew);
            */
           
            int count=3; 
            bool ret = false;
            char buf[512];
            while(count-->0)
            {
                printw("Enter password:");
                //std::cout <<"Enter Passwd:";
                //std::getline(std::cin, m_passwd);
                getnstr(buf, sizeof(buf));
                //if (m_passwd.compare(ptrPasswd) == 0)
                if (strncmp(buf, ptrPasswd, sizeof(buf)) == 0)
                {
                    ret = true;
                    break;
                }
            }

            echo();
            if (ret)
            {
            //    std::cout <<"Password right\n";
                printw("\nPassword right"); 
    sleep(10);
            }
            else
            {
                printw("\nPassword wrong");
    sleep(10);
            }
        }

        ~cPasswd()
        {
            //tcsetattr(STDIN_FILENO, TCSANOW, &m_old);
            echo();
        }

    private:
        std::string m_passwd;
        //struct termios m_old;
};

int main()
{
    initscr();
    cPasswd pwd;
    pwd.GetPasswd();
    endwin();
//    Func();
}
