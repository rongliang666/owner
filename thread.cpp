
#include "thread.h"

    void* RunThread(void* arg);
    int cThread::GetThreadNo()
    {
        return m_ThreadNo;
    }

       void cThread::SetThreadNo(uint32_t no)
       {
           m_ThreadNo = no;
       }

       bool cThread::IsProc(uint32_t no)
       {
           return (no==-1) || (m_ThreadNo==-1) || (m_ThreadNo==no);
       }

       void cThread::RestRunning()
       {
           m_bRunning = false;
       }

       void cThread::Start(bool detached, bool sscop)
       {
           Create(detached, sscop);
       }

       void cThread::Stop()
       {
           Cannel();
       }

       void cThread::Create(bool detached, bool sscop)
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

       void cThread::Detach()
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

       void cThread::Join()
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

       void cThread::Exit()
       {
           if (m_bRunning && (pthread_self()==m_ThreadId))
           {
               void* ret_val = NULL;
               pthread_exit(ret_val);
               m_bRunning = false;
           }
       }

       void cThread::Cannel()
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

       void cThread::Kill(int signo)
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

       void cThread::Sleep(long m_sec, long nano_sec)
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

void* RunThread(void* arg)
{
    if (arg != NULL)
    {
        ((cThread*)arg)->Run();
        ((cThread*)arg)->RestRunning();
    }
}

    int cMutex::Lock()
    {
        return pthread_mutex_lock(&m_Mutex);
    }

    int cMutex::UnLock()
    {
        return pthread_mutex_unlock(&m_Mutex);
    }

    int cMutex::TryLock()
    {
        return pthread_mutex_trylock(&m_Mutex);
    }


        void cCondition::Wait(cMutex& mutex)
        {
            pthread_cond_wait(&m_Cond, &mutex.m_Mutex);
        }

        void cCondition::Signal()
        {
            pthread_cond_signal(&m_Cond);
        }

        void cCondition::BroadCast()
        {
            pthread_cond_broadcast(&m_Cond);
        }


        int cSemaphore::Value()
        {
            return m_Value;
        }

        void cSemaphore::Init(int c)
        {
            m_Mutex.Lock();
            m_Value = c;
            m_Mutex.UnLock();
            m_Cond.Signal();
        }

        void cSemaphore::Inc()
        {
            m_Mutex.Lock();
            m_Value++;
            m_Mutex.UnLock();
            m_Cond.Signal();
        }

        void cSemaphore::Dec()
        {
            m_Mutex.Lock();
            while (m_Value<=0)
            {
                m_Cond.Wait(m_Mutex);
            }

            m_Value--;
            m_Mutex.UnLock();
        }

        int cCounter::Counter()
        {
            return m_Count;
        }

        void cCounter::Init(int c)
        {
            m_Mutex.Lock();
            m_Count = c;
            m_MutexList.clear();
            m_Mutex.UnLock();
        }

        void cCounter::Dec(int d, int c)
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

        void cCounter::ReleaseAll()
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

        int cThreadPool::cJob::JobNo()
        {
            return m_JobNo;
        }

cThreadPool::cPoolThread* cThreadPool::cJob::GetPoolThread()
       {
           return m_PoolThread;
       } 

       void cThreadPool::cJob::SetPoolThread(cPoolThread* thread)
       {
           m_PoolThread = thread;
       }

       bool cThreadPool::cJob::IsProc(int p)
       {
           return (p==-1)||(m_JobNo==-1)||(m_JobNo==p);
       }

        void cThreadPool::cPoolThread::SetEnd(bool var)
        {
            m_VarMutex.Lock();
            m_bEnd = var;
            m_VarMutex.UnLock();
        }

        void cThreadPool::cPoolThread::SetWork(bool var)
        {
            m_VarMutex.Lock();
            m_bWorking = var;
            m_VarMutex.UnLock();
        }

        void cThreadPool::cPoolThread::SetJob(cJob* job, void* arg)
        {
            m_VarMutex.Lock();
            m_Job = job;
            m_Data = arg;
            m_VarMutex.UnLock();
        }

cThreadPool::cJob* cThreadPool::cPoolThread::GetJob()
        {
            return m_Job;
        }

        bool cThreadPool::cPoolThread::IsWorking()
        {
            return m_Job!=NULL;
        }

        cCondition& cThreadPool::cPoolThread::GetWorkCond()
        {
            return m_WorkCond; 
        }

        cMutex& cThreadPool::cPoolThread::GetWorkMutex()
        {
            return m_WorkMutex;
        }

        cCondition& cThreadPool::cPoolThread::GetSyncCond()
        {
            return m_SyncCond;
        }

        cMutex& cThreadPool::cPoolThread::GetSyncMutex()
        {
            return m_SyncMutex;
        }

        void cThreadPool::cPoolThread::Run()
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


        void cThreadPool::Run(cJob* job, void* ptr)
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

        void cThreadPool::Sync(cJob* job)
        {
           if (!job)
           {
               return ;
           } 

           cPoolThread* thread = job->GetPoolThread();
           if (!thread)
           {
               return ;
           }

           thread->GetSyncMutex().Lock();
           thread->SetJob(NULL, NULL);
           thread->GetSyncMutex().UnLock();

           job->SetPoolThread(NULL);
        }

        void cThreadPool::SyncAll()
        {
            for (int i=0; i<m_MaxNum; i++)
            {
                if (m_vecThread[i]->GetSyncMutex().TryLock())
                {
                    m_vecThread[i]->GetSyncMutex().Lock();
                }

                m_vecThread[i]->GetSyncMutex().UnLock();
            }
        }

cThreadPool::cPoolThread* cThreadPool::GetIdle()
        {
            cPoolThread* ptr = NULL;
            while (true)
            {
                m_IdleMutex.Lock();
                while (m_listIdleThead.size() == 0)
                {
                    m_IdleCond.Wait(m_IdleMutex);
                }

                if (m_listIdleThead.size() > 0)
                {
                    ptr = m_listIdleThead.back();
                    m_listIdleThead.pop_back();
                }

                m_IdleMutex.UnLock();
                return ptr;
            }
        }

        void cThreadPool::AppendIdle(cPoolThread* ptr)
        {
            std::list<cPoolThread*>::iterator iter;
            m_ListMutex.Lock();
            for (iter=m_listIdleThead.begin(); iter!=m_listIdleThead.end(); iter++)
            {
                if (*iter == ptr)
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


