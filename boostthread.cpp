#include "boost/thread.hpp"
#include <unistd.h>
#include <iostream>
#include <boost/thread/condition.hpp>
#include <string>

//boost thread
class cThread
{
    public:
        cThread(int i):m_mem(i)
        {}

        cThread()
        {}

        void operator ()(int i=10000)
        {
            m_mem = i;
            sleep(2);
            std::cout <<"in cThread i=" << m_mem << "\n";
        }

    private:
        int m_mem;
};

void func(int i)
{
    sleep(5);
    boost::thread::id id = boost::this_thread::get_id();
    std::cout <<"in func thread id =" << id <<"\n";
}

class cAccount
{
    public:
        void Dec(int i)
        {
            boost::unique_lock<boost::shared_mutex> lock(m_Mutex);
            m_iAccount -= i;    
            std::cout <<"in Dec: " << m_iAccount << "\n";
        }

        void Inc(int i)
        {
            boost::unique_lock<boost::shared_mutex> lock(m_Mutex);
            m_iAccount += i;
            std::cout <<"in Dec: " << m_iAccount << "\n";
        }

        cAccount(int i):m_iAccount(i)
    {}

        int GetAccout()
        {
            boost::shared_lock<boost::shared_mutex> lock(m_Mutex);
            return m_iAccount;
        }

        void Lock()
        {
            m_out.lock();
        }

        void UnLock()
        {
            m_out.unlock();
        }

    private:
        boost::shared_mutex m_Mutex;
        int m_iAccount;

        boost::mutex m_out;
};

cAccount account(100);
void Dec()
{
    account.Lock();
    for (int i=0; i<10; i++)
    {
        account.Dec(1);
    }

    account.UnLock();
}

void Inc()
{
    account.Lock();
    for (int i=0; i<10; i++)
    {
        account.Inc(1);
    }

    account.UnLock();
}

typedef boost::mutex::scoped_lock scoped_lock;
boost::mutex ioMutex;
const int BUFF_SIZE = 10;
class cBuffer
{
    public:
        cBuffer():count(0),pPos(0), gPos(0)
        {
            memset(buf, 0, sizeof(buf));
        }

        void Put(int s)
        {
            scoped_lock lock(m_Mutex);    

            if (count >= BUFF_SIZE)
            {
                {
                    scoped_lock lock(ioMutex);
                    std::cout <<"buf is full in Put, count is : " << count  << std::endl;
                }
                m_Cond.wait(lock);
            }

            buf[pPos++%BUFF_SIZE] = s;
            count++;
            m_Cond.notify_one();
        }

        void Get(int&  c)
        {
            scoped_lock lock(m_Mutex);
            if (count == 0)
            {
                {
                    scoped_lock lock(ioMutex);
                    std::cout <<"buf is empty in Get, count is : " << count << std::endl;
                }
                m_Cond.wait(lock);
            }

            c = buf[gPos++%BUFF_SIZE];
            count--;
            m_Cond.notify_one();
        }

    private:
        boost::mutex m_Mutex;
        boost::condition m_Cond;
        int count ;
        int pPos;
        int gPos;
        int buf[BUFF_SIZE];
};

cBuffer buff;

const int TOTAL = 100;
void Writer()
{
   for (int i=0; i<TOTAL; i++)
   {
       buff.Put(i);
       scoped_lock lock(ioMutex);
       std::cout <<"Writer " << i << std::endl;
   } 
}

void Read()
{
    for (int i=0; i<TOTAL; i++)
    {
        int c;
        buff.Get(c);
       scoped_lock lock(ioMutex);
        std::cout <<"Read " << c << std::endl;
    }
}

int main()
{
    
    boost::thread Produce(Writer);
    boost::thread Consumer(Read);

    Produce.join();
    Consumer.join();
    /*
        cThread c(1000);
        boost::thread::attributes attr;
        attr.set_stack_size(1024*1024*10);

        boost::function<void()> f = boost::bind(func, 100);
        
        boost::thread t(attr, f);

    if (!t.timed_join(boost::posix_time::seconds(1)))
    {
        std::cout <<"1000 milliseconds reached.\n";
    }

    std::cout << "main thread id :" << boost::this_thread::get_id() << std::endl;
    t.join();
    */

    return 0;
    boost::thread t1(Dec);
    boost::thread t2(Inc);

    t1.join();
    t2.join();
    return 0;
}


