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

       int GetThreadNo();

       void SetThreadNo(uint32_t no);

       bool IsProc(uint32_t no);

       void RestRunning();

       virtual void Run() = 0;
       virtual void Start(bool detached=false, bool sscop=false);

       virtual void Stop();

       void Create(bool detached=false, bool sscop=false);

       void Detach();

       void Join();
       void Exit();
       void Cannel();

       void Kill(int signo);

       void Sleep(long m_sec, long nano_sec);
};

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

    int Lock();

    int UnLock();

    int TryLock();

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

        void Wait(cMutex& mutex);

        void Signal();

        void BroadCast();
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

        int Value();

        void Init(int c);
        void Inc();

        void Dec();
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

        int Counter();

        void Init(int c);

        void Dec(int d, int c);

        void ReleaseAll();
};




class cThreadPool
{
    public:
    
        class cPoolThread;
    class  cJob
    {
        protected:
            int m_JobNo;
            cThreadPool::cPoolThread* m_PoolThread;

        public:
            cJob(int n):m_JobNo(n), m_PoolThread(NULL)
        {}
            virtual void Run(void* ptr)=0;
            int JobNo();
            cPoolThread* GetPoolThread();
           void SetPoolThread(cPoolThread* thread);

           bool IsProc(int p);
    };

    class cPoolThread:public cThread 
    {
        protected:
            cThreadPool* m_Pool;
            cThreadPool::cJob* m_Job;
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
    
            void SetEnd(bool var);
            void SetWork(bool var);
    
            void SetJob(cJob* job, void* arg);
    
            cJob* GetJob();
    
            bool IsWorking();
    
            cCondition& GetWorkCond();
    
            cMutex& GetWorkMutex();
    
            cCondition& GetSyncCond();
    
            cMutex& GetSyncMutex();
    
            void Run();
    };

        protected:
        int m_MaxNum;

        std::vector<cThreadPool::cPoolThread*> m_vecThread;
        std::list<cThreadPool::cPoolThread*> m_listIdleThead;
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
            m_vecThread[i]->Start(true, true);
        }

        pthread_setconcurrency(n+pthread_getconcurrency());
    }

        ~cThreadPool()
        {
            SyncAll();
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

        void Run(cJob* job, void* ptr);
        void Sync(cJob* job);
        cPoolThread* GetIdle();
        void AppendIdle(cPoolThread* ptr);
        void SyncAll();
};

#endif

