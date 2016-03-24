#ifndef __THREAD_H__
#define __THREAD_H__

#include <stdio.h>
#include <pthread.h>
#include <vector>
#include <list>
#include <stdint.h>
#include <time.h>
#include <iostream>
#include <signal.h>


void* RunThread(void* arg);
class cThread
{
    protected:
        pthread_t m_ThreadId;
        pthread_attr_t m_ThreadAttr;
        bool m_bRunning;
        int m_ThreadNo;

    public:
       cThread(int no=-1):m_ThreadNo(no), m_bRunning(false) 
    {} 

       virtual ~cThread()
       {
           if (m_bRunning)
           {
               Cannel();
           }
       }

       int GetThreadNo()const
       {
           return m_ThreadNo;
       }

       void SetThreadNo(uint32_t no)
       {
           m_ThreadNo = no;
       }

       bool IsProc(uint32_t no)
       {
           return (no==-1) || (m_ThreadNo==-1) || (m_ThreadNo==no);
       }

       void RestRunning()
       {
           m_bRunning = false;
       }

       virtual void Run() = 0;
       virtual void Start(bool detached=false, bool sscop=false)
       {
           Create(detached, sscop);
       }

       virtual void Stop()
       {
           Cannel();
       }

       void Create(bool detached=false, bool sscop=false)
       {
           if (!m_bRunning)
           {
               int ret;
               if (ret=pthread_attr_init(&m_ThreadAttr) != 0)
               {
                    std::cout << "pthread_attr_init failed.\n";
                    return ;
               }

               if (detached)
               {
                   if (ret=pthread_attr_setdetachstate(&m_ThreadAttr, PTHREAD_CREATE_DETACHED) != 0)
                   {
                       std::cout <<"pthread_attr_setdetachstate failed.\n";
                        return ;   
                   }
               }

               if (sscop)
               {
                   if (ret=pthread_attr_setscope(&m_ThreadAttr, PTHREAD_SCOPE_SYSTEM)!=0)
                   {
                       std::cout << "pthread_attr_setscope failed.\n";
                       return ;
                   }
               }

               if (ret=pthread_create(&m_ThreadId, &m_ThreadAttr, RunThread, this) !=0)
               {
                   std::cout <<"pthread_create failed.\n";
                   return ;
               }

               m_bRunning = true;
           }
           else
           {
               std::cout <<"thread is already running\n";
           }
       }

       void Detach()
       {
           if (m_bRunning)
           {
               int ret;
               if (ret=pthread_detach(m_ThreadId) != 0)
               {
                   std::cout <<"pthread_detach failed.\n";
                   return ;
               }
           } 
       }

       void Join()
       {
           if (m_bRunning)
           {
               int ret;
               if (ret=pthread_join(m_ThreadId, NULL)!=0)
               {
                    std::cout <<"pthread_join failed.\n";
                    return;
               }

               m_bRunning = false;
           }
       }

       void Exit()
       {
           if (m_bRunning && (pthread_self()==m_ThreadId))
           {
               void* ret_val = NULL;
               pthread_exit(ret_val);
               m_bRunning = false;
           }
       }

       void Cannel()
       {
            if (m_bRunning)
            {
                int ret;
                if (ret=pthread_cancel(m_ThreadId) != 0)
                {
                    std::cout <<"pthread_cancel failed\n";
                    return;
                }
            }
       }

       void Kill(int signo)
       {
           if (m_bRunning)
           {
               int ret;
               if (ret=pthread_kill(m_ThreadId, signo) != 0)
               {
                   std::cout <<"pthread_kill failed.\n";
                   return ;
               }

               m_bRunning = false;
           }
       }

       void Sleep(long m_sec, long nano_sec)
       {
           if (m_bRunning)
           {
               struct timespec interval;
               if (m_sec < 0)
               {
                   m_sec = 0;
               }

               if (nano_sec<0)
               {
                   nano_sec = 0;
               }

               while(nano_sec >= 1000000)
               {
                   m_sec++;
                   nano_sec -= 1000000;
               }

               interval.tv_sec = m_sec/1000;
               interval.tv_nsec = (m_sec%1000)*1000000 + nano_sec;
               nanosleep(&interval, 0);

           }
       }
};

void* RunThread(void* arg)
{
    if (arg != NULL)
    {
        ((cThread*)arg)->Run();
        ((cThread*)arg)->RestRunning();
    }
}

class cMutex
{
    protected:
    pthread_mutex_t m_Mutex;
    pthread_mutexattr_t m_MutexAttr;

    public:
    cMutex()
    {
        pthread_mutexattr_init(&m_MutexAttr);
        pthread_mutex_init(&m_Mutex, &m_MutexAttr);
    }

    ~cMutex()
    {
        pthread_mutex_destroy(&m_Mutex);
        pthread_mutexattr_destroy(&m_MutexAttr);
    }

    int Lock()
    {
        return pthread_mutex_lock(&m_Mutex);
    }

    int UnLock()
    {
        return pthread_mutex_unlock(&m_Mutex);
    }

    int TryLock()
    {
        return pthread_mutex_trylock(&m_Mutex);
    }

    friend class cCondition;
};

class cCondition
{
    protected:
        pthread_cond_t m_Cond;

    public:
        cCondition()
        {
            pthread_cond_init(&m_Cond, NULL);
        }

        ~cCondition()
        {
            pthread_cond_destroy(&m_Cond);
        }

        void Wait(cMutex& mutex)
        {
            pthread_cond_wait(&m_Cond, &mutex.m_Mutex);
        }

        void Signal()
        {
            pthread_cond_signal(&m_Cond);
        }

        void BroadCast()
        {
            pthread_cond_broadcast(&m_Cond);
        }
};

class cSemaphore
{
    protected:
        int m_Value;
        cMutex m_Mutex;
        cCondition m_Cond;

    public:
        cSemaphore(int c = 0):m_Value(c)
    {}
        ~cSemaphore()
        {}

        int Value()const
        {
            return m_Value;
        }

        void Init(int c)
        {
            m_Mutex.Lock();
            m_Value = c;
            m_Mutex.UnLock();
            m_Cond.Signal();
        }

        void Inc()
        {
            m_Mutex.Lock();
            m_Value++;
            m_Mutex.UnLock();
            m_Cond.Signal();
        }

        void Dec()
        {
            m_Mutex.Lock();
            while (m_Value<=0)
            {
                m_Cond.Wait(m_Mutex);
            }

            m_Value--;
            m_Mutex.UnLock();
        }
};

class cCounter
{
    protected:
        int m_Count;
        std::list<cMutex*> m_MutexList;
        cMutex m_Mutex;

    public:
        cCounter(int c=-1):m_Count(c)
    {}

        ~cCounter()
        {}

        int Counter()const
        {
            return m_Count;
        }

        void Init(int c)
        {
            m_Mutex.Lock();
            m_Count = c;
            m_MutexList.clear();
            m_Mutex.UnLock();
        }

        void Dec(int d, int c)
        {
            m_Mutex.Lock();
            m_Count -= d;
            
            if (m_Count > 0)
            {
                cMutex* ptr = new cMutex;
                if (ptr != NULL)
                {
                    m_MutexList.push_back(ptr);
                    ptr->Lock();
                }

                m_Mutex.UnLock();
            }
            else
            {
                ReleaseAll();
                m_Count = c;
                m_Mutex.UnLock();
            }
        }

        void ReleaseAll()
        {
            cMutex* ptr = NULL;
            m_Mutex.Lock();
            while (m_MutexList.size() > 0)
            {
                ptr = m_MutexList.back();
                ptr->UnLock();
                m_MutexList.pop_back();
            }
            m_Mutex.UnLock();
        }
};

class cThreadPool;
class cJob;

class cPoolThread:public cThread 
{
    protected:
        cThreadPool* m_Pool;
        cJob* m_Job;
        void* m_Data;
        cCondition m_WorkCond;
        cCondition m_SyncCond;
        cMutex m_WorkMutex;
        cMutex m_SyncMutex;

        bool m_bWorking;
        bool m_bEnd;

        cMutex m_VarMutex;
        cMutex m_DelMutex;

    public:
        cPoolThread(int n, cThreadPool* pool):cThread(n), m_Pool(pool), m_Job(NULL), m_Data(NULL), m_bWorking(false), m_bEnd(false)
    {}

        ~cPoolThread()
        {}

        void SetEnd(bool var)
        {
            m_VarMutex.Lock();
            m_bEnd = var;
            m_VarMutex.UnLock();
        }

        void SetWork(bool var)
        {
            m_VarMutex.Lock();
            m_bWorking = var;
            m_VarMutex.UnLock();
        }

        void SetJob(cJob* job, void* arg)
        {
            m_VarMutex.Lock();
            m_Job = job;
            m_Data = arg;
            m_VarMutex.UnLock();
        }

        cJob* GetJob()
        {
            return m_Job;
        }

        bool IsWorking()
        {
            return m_Job!=NULL;
        }

        cCondition& GetWorkCond()
        {
            return m_WorkCond; 
        }

        cMutex& GetWorkMutex()
        {
            return m_WorkMutex;
        }

        cCondition& GetSyncCond()
        {
            return m_SyncCond;
        }

        cMutex& GetSyncMutex()
        {
            return m_SyncMutex;
        }

        void Run()
        {
            m_DelMutex.Lock();

            while (!m_bEnd)
            {
                m_WorkMutex.Lock();

                while (m_Job!=NULL && !m_bEnd)
                {
                    m_WorkCond.Wait(m_WorkMutex);
                }

                m_WorkMutex.UnLock();

                if (m_Job != NULL)
                {
                    m_Job->Run(m_Data);
                    m_Job->SetPoolThread(NULL);
                    SetJob(NULL, NULL);

                    m_SyncMutex.UnLock();
                }

                m_Pool->AppendIdle(this);
            }

            m_DelMutex.UnLock();
        }
};

class  cJob
{
    protected:
        int m_JobNo;
        cPoolThread* m_PoolThread;

    public:
        cJob(int n):m_JobNo(n), m_PoolThread(NULL)
    {}
        virtual Run(void* ptr)=0;
        int JobNo()const
        {
            return m_JobNo;
        }

       cPoolThread* GetPoolThread()const
       {
           return m_PoolThread;
       } 

       void SetPoolThread(cPoolThread* thread)
       {
           m_PoolThread = thread;
       }

       bool IsProc(int p)
       {
           return (p==-1)||(m_JobNo==-1)||(m_JobNo==p);
       }
};

class cThreadPool
{
    protected:
        int m_MaxNum;

        std::vector<cPoolThread*> m_vecThread;
        std::list<cPoolThread*> m_listIdleThead;
        cMutex m_IdleMutex;
        cMutex m_ListMutex;
        cCondition m_IdleCond;

    public:
        cThreadPool(int n):m_MaxNum(n)
    {
        for (int i=0; i<n; i++)
        {
            m_vecThread[i] = new cPoolThread(i, this);
            m_listIdleThead.push_back(m_vecThread[i]);
            m_vecThread[i]->Start(true, ture);
        }

        pthread_setconcurrency(n+pthread_getconcurrency());
    }

        ~cThreadPool()
        {
            SyncCall();
            for (int i=0; i<m_MaxNum; i++)
            {
                m_vecThread[i]->GetSyncMutex().Lock();
                m_vecThread[i]->SetEnd(true);
                m_vecThread[i]->SetJob(NULL, NULL);
                m_vecThread[i]->GetWorkMutex().Lock();
                m_vecThread[i]->GetWorkCond().Signal();
                m_vecThread[i]->GetWorkMutex().UnLock();
                m_vecThread[i]->GetSyncMutex().UnLock();
            }

            for (int i=0; i<m_MaxNum; i++)
            {
                delete m_vecThread[i];
                m_vecThread[i] = NULL;
            }
        }

        void Run(cJob* job, void* ptr)
        {
            if (!job)
            {
                return;
            }

            cPoolThread* thread = GetIdle();
            thread->GetSyncMutex().Lock();
            thread->SetJob(job, ptr);
            job->SetPoolThread(thread);
            thread->GetWorkMutex().Lock();
            thread->GetWorkCond().Signal();
            thread->GetWorkMutex().UnLock();
        }

        void Sync(cJob* job)
        {
           if (!job)
           {
               return ;
           } 

           cPoolThread* thread = job.GetPoolThread();
           if (!thread)
           {
               return ;
           }

           thread->GetSyncMutex().Lock();
           thread->SetJob(NULL, NULL);
           thread->GetSyncMutex().UnLock();

           job->SetPoolThread(NULL);
        }

        void SyncAll()
        {
            for (int i=0; i<m_MaxNum; i++)
            {
                if (m_PoolThread[i]->GetSyncMutex().TryLock())
                {
                    m_PoolThread[i]->GetSyncMutex().Lock();
                }

                m_PoolThread[i]->GetSyncMutex().UnLock();
            }
        }

        cPoolThread* GetIdle()
        {
            cPoolThread* ptr = NULL;
            while (true)
            {
                m_IdleMutex.Lock();
                while (m_listIdleThead.size() == 0)
                {
                    m_IdleCond.Wait(m_IdleCond);
                }

                if (m_listIdleThead.size() > 0)
                {
                    ptr = m_listIdleThead.pop_back();
                }

                m_IdleMutex.UnLock();
                return ptr;
            }
        }

        void AppendIdle(cPoolThread* ptr)
        {
            std::list<cPoolThread*>::iterator iter;
            m_ListMutex.Lock();
            for (iter=m_PoolThread.begin(); iter!=m_PoolThread.end(); iter++)
            {
                if (iter == ptr)
                {
                    m_ListMutex.UnLock();
                    return;
                }
            }

            m_listIdleThead.push_back(ptr);
            m_ListMutex.UnLock();

            m_IdleMutex.Lock();
            m_IdleCond.BroadCast();
            m_IdleMutex.UnLock();
        }
};

#endif

