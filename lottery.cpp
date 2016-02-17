#include <iostream>
#include <sys/time.h>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <algorithm>

class cBall
{
    public:
        cBall(int a):m_iNum(a)
    {}

        bool IsRed()
        {
            return m_bIsRed;
        }

        int GetNum()
        {
            return m_iNum;
        }

        bool operator==(int a)
        {
            return m_iNum == a;
        }

        bool operator==(cBall& a)
        {
            return m_iNum == a.m_iNum;
        }

        bool operator<(int a)
        {
            return m_iNum < a;
        }

        bool operator<(cBall& a)
        {
            return m_iNum < a.m_iNum;
        }
        

    protected:
        int m_iNum;
        bool m_bIsRed;
};

class cWhiteBall:public cBall
{
    public:
        cWhiteBall(int a):cBall(a)
    {
        m_bIsRed = false;
    }
};

class cRedBall:public cBall
{
    public:
        cRedBall(int a):cBall(a)
    {
        m_bIsRed = true;
    }
};

static int iseed;
class cRandom
{
    public:
        cRandom(int min, int max, int num)
    {
        srand(time(NULL)+iseed);
        for (int i=0; i<num; i++)
        {
            int iRand = rand()%(max-min+1) + min;
            std::vector<int>::const_iterator constIter = find(m_vecNum.begin(), m_vecNum.end(), iRand);
            if (constIter != m_vecNum.end())
            {
                i--;
                continue;
            }

            iseed = iRand;
            m_vecNum.push_back(iRand);
        }
    }

        std::vector<int> GetRandNum()
        {
            return m_vecNum;
        }

    private:
        std::vector<int> m_vecNum;
};

class cTickets
{
    public:
        cTickets()
        {
            cRandom Random(1, 58, 6);
            m_vecNum = Random.GetRandNum();
            sort(m_vecNum.begin(), m_vecNum.end());
        }

        std::vector<int> GetNum()
        {
            return m_vecNum;
        }

        void Display()
        {
            std::cout <<"This Ticket is:\n";
            std::vector<int>::iterator iter;
            for (iter=m_vecNum.begin(); iter!=m_vecNum.end(); iter++)
            {
                std::cout << *iter << '\t';
            }

            std::cout << "\n Ticket end...\n";
        }

    private:
        std::vector<int> m_vecNum;
};

class cLottery
{
    public:
        cLottery()
        {
            cRandom RandomWhite(1, 58, 5);
            std::vector<int> vecNum = RandomWhite.GetRandNum();
            std::vector<int>::iterator iter;
            for (iter=vecNum.begin(); iter!=vecNum.end(); iter++)
            {
                m_ptrBall.push_back(new cWhiteBall(*iter));
            }

            cRandom RandomRed(1, 34, 1);
            vecNum = RandomRed.GetRandNum();
            m_ptrBall.push_back(new cRedBall(vecNum[0]));
        }

        virtual ~cLottery()
        {
            for (int i=0; i<m_ptrBall.size(); i++)
            {
                if (m_ptrBall[i] != NULL)
                {
                    delete m_ptrBall[i];
                }
            }
        }

        std::vector<cBall*> GetBall()
        {
            return m_ptrBall;
        }

        void Display()
        {
            std::cout << "Lottery is:\n";
            std::vector<cBall*>::iterator iter;
            for (iter=m_ptrBall.begin(); iter!=m_ptrBall.end(); iter++)
            {
                if ((*iter)->IsRed())
                {
                    std::cout <<"this ball is red\tNum is:" << (*iter)->GetNum() << std::endl;
                }
                else
                {
                    std::cout <<"Num is:" << (*iter)->GetNum() << std::endl;
                }
            }
        }

    private:
        std::vector<cBall*> m_ptrBall;
};

class cWinner
{
    public:
        cWinner(std::vector<cTickets*> vecTickets, cLottery& lottery)
        {
            std::vector<cTickets*>::iterator iter;
            std::vector<cBall*> vecBall = lottery.GetBall();

            for (iter=vecTickets.begin(); iter!=vecTickets.end(); iter++)
            {
                std::vector<int> vecNum = (*iter)->GetNum();
                std::vector<int>::iterator iterNum;
                int imatch = 0;
                bool bIsRed =false;
                for (iterNum=vecNum.begin(); iterNum!=vecNum.end(); iterNum++)
                {
                    std::vector<cBall*>::iterator iterBall;
                    for (iterBall=vecBall.begin(); iterBall!=vecBall.end(); iterBall++)
                    {
                        if ((*iterBall)->GetNum() == *iterNum)
                        {
                            imatch++;
                            if ((*iterBall)->IsRed())
                            {
                                bIsRed = true;
                            }
                        }
                    }
                }

                m_iMatch.push_back(imatch);
                m_bIsRed.push_back(bIsRed);
            }
        }

        void DisPlay()
        {
            std::cout<< "DisPlay the winner result\n";
            for (int i=0; i<m_iMatch.size(); i++)
            {
                std::cout<< "got match num: " << m_iMatch[i] << std::endl;
                if (m_bIsRed[i])
                {
                    std::cout << "has got red ball match\n";
                }
                else
                {
                    std::cout << "but not got red ball match\n";
                }
            }
        }

    private:
        std::vector<int> m_iMatch;
        std::vector<bool> m_bIsRed;
};

class cGame
{
    public:
        cGame()
        {}

        virtual ~cGame()
        {
            for (int i=0; i<m_ptrTickets.size(); i++)
            {
                if (m_ptrTickets[i] != NULL)
                {
                    delete m_ptrTickets[i];
                }
            }
        }

        void Run()
        {
            Menu();
            Play();
        }

    private:
        void CreateTicket(int n)
        {
            for (int i=0; i<n; i++)
            {
                m_ptrTickets.push_back(new cTickets);
            }
        }

        void Menu()
        {
            int iNumTicket;
            std::cout << "Welcome to the PowerBall Lottery!" << std::endl;
            std::cout << "How many tickets would you like? " << std::endl;

            while(1)
            {
                 std::cout << "Enter amount of tickets you would like to purchase: ";
                 std::cin >> iNumTicket;
                 std::cin.sync();
                 if (iNumTicket < 1 || iNumTicket > 100)
                 {
                     std::cout << "Ticket num between [1, 100]\n";
                 }
                 else
                 {
                     break;
                 }
            }

            CreateTicket(iNumTicket);
            std::cout << "Your tickets are registered. Thank you for playing the PowerBall lottery!" << std::endl;
        }

        void Play()
        {
            std::cout << "Let\'s see this weeks PowerBall lottery numbers!" << std::endl;
            m_lottery.Display();
            std::cout << "your ticket:\n";
            for (int i=0; i<m_ptrTickets.size(); i++)
            {
                m_ptrTickets[i]->Display();
            }
                
            cWinner win(m_ptrTickets, m_lottery);
            win.DisPlay();
        }

    private:
        std::vector<cTickets*> m_ptrTickets;
        cLottery m_lottery;
};

class cTimer
{
    private:
        struct timeval m_tStart;
        struct timeval m_tEnd;

    public:
        cTimer()
        {
            gettimeofday(&m_tStart, NULL);
        }

        void End()
        {
            gettimeofday(&m_tEnd, NULL);
        }

        void Display()
        {
            struct timeval diff;
            timersub(&m_tEnd, &m_tStart, &diff);
            std::cout << "game constructor time. sec:" <<diff.tv_sec<< "\tusec:" << diff.tv_usec<< std::endl; 
        }
};

/* 计算代码段执行时间的类(单位为micro sec) */
class CTimeCost
{
    public:
        CTimeCost()
        {   
            reset();
        }   

        ~CTimeCost()
        {   
        }   

        void reset()
        {   
            gettimeofday(&m_tv, NULL);
            m_tStartTime = m_tv.tv_sec * 1000000 + m_tv.tv_usec;
        }   

        time_t value() 
        {   
            gettimeofday(&m_tv, NULL);
            m_tEndTime = m_tv.tv_sec * 1000000 + m_tv.tv_usec ;
            return (m_tEndTime - m_tStartTime);
        }   

    private:
        time_t m_tStartTime;
        time_t m_tEndTime;
        timeval m_tv;
};

int main(void)
{
    CTimeCost  timer;
    cGame game1, game2;
    time_t t = timer.value();
    std::cout <<"cost t " << t <<std::endl;
    
    game1.Run();

    return 0;
}

