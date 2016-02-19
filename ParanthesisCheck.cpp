#include <iostream>
#include <list>

class cStack
{
    public:
        cStack()
        {
            m_lStack.clear();
        }

        void Push(char  c)
        {
            m_lStack.push_back(c);
        }

        char Pop()
        {
            char c = m_lStack.back();
            m_lStack.pop_back(); 
            return c;
        }

        int Size()
        {
            return m_lStack.size();
        }

        int Empty()
        {
            return Size()==0;
        }

    private:
        std::list<char> m_lStack;
};

class cNode
{
    public:
        cNode(char c):m_c(c)
    {}

        virtual bool IsPre()=0;
        virtual bool IsLatter()=0;
        virtual bool IsMatch(char c)=0;
    protected:
        char m_c;
};

class cLarge:public cNode
{
    public:
        cLarge(char c):cNode(c)
    {}
        bool IsPre()
        {
            if (m_c == '{')
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        bool IsLatter()
        {
            if (m_c == '}')
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        bool IsMatch(char c)
        {
            if ((m_c=='{'&&c=='}') || (m_c=='}'&&c=='{'))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
};

class cMiddle:public cNode
{
    public:
        cMiddle(char c):cNode(c)
    {}
        bool IsPre()
        {
            if (m_c == '[')
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        bool IsLatter()
        {
            if (m_c == ']')
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        bool IsMatch(char c)
        {
            if ((m_c=='['&&c==']') || (m_c==']'&&c=='['))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
};

class cSmall:public cNode
{
    public:
        cSmall(char c):cNode(c)
    {}
        bool IsPre()
        {
            if (m_c == '(')
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        bool IsLatter()
        {
            if (m_c == ')')
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        bool IsMatch(char c)
        {
            if ((m_c=='('&&c==')') || (m_c==')'&&c=='('))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
};

class cPair
{
    public:
        cPair():m_node(NULL)
        {}

        ~cPair()
        {
            if (m_node)
            {
                delete m_node;
                m_node = NULL;
            }
        }

        bool Init(char c)
        {
            bool ret = true;
            switch (c)
            {
                case '{':
                case '}':
                    m_node = new cLarge(c);
                    break;

                case '[':
                case ']':
                    m_node = new cMiddle(c);
                    break;

                case '(':
                case ')':
                    m_node = new cSmall(c);
                    break;

                default:
                   ret = false; 
            }

            return ret;
        }
        
        bool IsPre()
        {
            return m_node->IsPre();
        }

        bool IsLatter()
        {
            return m_node->IsLatter();
        }

        bool IsMatch(char c)
        {
            return m_node->IsMatch(c);
        }

    private:
        cNode* m_node;
};

class cTest
{
    public:
        cTest(std::string& str):m_str(str)
    {}

        void DoRun()
        {
            if (DoCheck())
            {
                std::cout <<"string is ok. string:" << m_str << std::endl;
            }
            else
            {
                std::cout <<"string is not ok. string:" << m_str<< std::endl;
            }
        }

    private:
        bool DoCheck(void)
        {
            for (int i=0; i<m_str.size(); i++)
            {
                cPair c;
                if (c.Init(m_str[i]))
                {
                    if (c.IsPre())
                    {
                        m_stack.Push(m_str[i]);
                        continue;
                    }
                    else if(c.IsLatter())
                    {
                        if (m_stack.Empty())
                        {
                            return false;
                        }

                        if (c.IsMatch(m_stack.Pop()))
                        {
                            continue;
                        }
                        else
                        {
                            return false;
                        }
                    }
                    else
                    {
                        continue;
                    }
                }
            }

            if (m_stack.Empty())
            {
                return true;
            }
            else
            {
                return false;
            }
        }

    private:
        cStack m_stack;
        std::string m_str;
};

int main(void)
{
    std::string str("abcx{xxxx");
    cTest test1(str);
    test1.DoRun();

    str = "abcx{x[xxx";
    cTest test2(str);
    test2.DoRun();

    str = "abcx{x[xxx}]";
    cTest test3(str);
    test3.DoRun();

    str = "xxxxxxxxx{xafskf[fskl(fkjalk) xxx]xxxxxxxx}dfkajfdka";
    cTest test4(str);
    test4.DoRun();
}

