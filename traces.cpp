#include <iostream>
#include <cxxabi.h>
#include <sstream>
#include <execinfo.h>
#include <string>
#include <stdlib.h>

const int DUMP_STACK_DEPTH_MAX =  50;
template<class TI, class TO>
TO ToType (TI a)
{
    std::stringstream ss;
    ss << a;
    TO b;
    ss >> b;
    return b;
}

class cStrace
{
    public:
        cStrace()
        {
            stack_strings = NULL;
            for (int i=0; i<DUMP_STACK_DEPTH_MAX; i++)
            {
                stack_trace[i] = NULL;
            }
        }

        void DoProcess()
        {
                depth = backtrace(stack_trace, DUMP_STACK_DEPTH_MAX);
                stack_strings = (char**)backtrace_symbols(stack_trace, depth);
                if (stack_strings == NULL)
                {
                    m_sErrMsg += " Memory is not enough while dump Stack Trace! \n";
                    return;
                }

                m_sSrc+="Backtrace:\n";
                for (int i=0; i<depth; i++)
                {
                    m_sSrc+= ToType<int, std::string>(i);
                    m_sSrc+="\t";
                    m_sSrc+= stack_strings[i];
                    m_sSrc+= "\n";

                    m_sDst += "\n";
                    demangle(stack_strings[i], m_sDst);
                }
        }

        void demangle(char * msg,std::string& out)
        {
            char *mangled_name = 0, *offset_begin = 0, *offset_end = 0,*src=msg;
              for (char *p = msg; *p; ++p)
              {
                  if (*p == '(')
                  {
                      mangled_name = p; 
                  }
                  else if(*p == '+') 
                  {
                        offset_begin = p;
                  }
                  else if(*p == ')')
                  {
                      offset_end = p;
                      break;
                  }

               }

                  if (mangled_name && offset_begin && offset_end && mangled_name < offset_begin)
                  {
                      *mangled_name++ = '\0';
                      *offset_begin++ = '\0';
                      *offset_end++ = '\0';
                      int status;

                      char * real_name = abi::__cxa_demangle(mangled_name, 0, 0, &status);
                      if (status == 0)
                      {
                          out = out + real_name + "+" + offset_begin + offset_end;
                      }
                      else
                      {
                          out = out + mangled_name + "+" + offset_begin + offset_end;
                      }

                      free(real_name);
                  }
                  else
                  {
                    out += msg;
                  }
        }

        ~cStrace()
        {
            if (stack_strings != NULL)
            {
                free(stack_strings);
                stack_strings = NULL;
            }
        }

        void Display()
        {
            std::cout <<m_sSrc;    
            std::cout <<"---------------------------\n";
            std::cout <<m_sDst;    
        }
        
    private:
        std::string m_sErrMsg;
        std::string m_sSrc;
        std::string m_sDst;
        int depth;
        void *stack_trace[DUMP_STACK_DEPTH_MAX];
        char **stack_strings;

};

class cTest
{
    public:
        void DoRun(int nCall)
        {
            if (nCall > 1)
            {
                DoRun(--nCall); 
            }
            else
            {
                Func();
            }
        }

        void Func()
        {
            trace.DoProcess();
            trace.Display();
        }

    private:
        cStrace trace;
};

int main()
{
    cTest tst;
    tst.DoRun(10);
}
