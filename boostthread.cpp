#include "boost/thread.hpp"
#include <unistd.h>
#include <iostream>

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

int main()
{
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

    boost::thread t1(Dec);
    boost::thread t2(Inc);

    t1.join();
    t2.join();
    return 0;
}


