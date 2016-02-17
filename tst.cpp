#include <string>
#include <vector>
#include <algorithm>
#include <typeinfo>
#include <iostream>
#include <climits>
#include <memory>
#include <string.h>
#include <stdexcept>
#include <pthread.h>
#include <functional>
#include <list>
//#include "Attr_API.h"
#include "curl/curl.h"
#include <map>

using namespace std;

typedef std::runtime_error Error;
class employe
{
    private:
        string first_name;
        string second_name;
        string FullName(void)const;

    public:
        virtual void Print(void)const;
        void InitName(const string& first, const string& second);
        employe(const string& f, const string& s):first_name(f), second_name(s){}
        employe(const employe& src):first_name(src.first_name), second_name(src.second_name){}
        employe& operator=(const employe& src)
        {
            first_name = src.first_name;
            second_name = src.second_name;
        }
};

void employe::InitName(const string& first, const string& second)
{
    first_name = first;
    second_name = second;
}

string employe::FullName(void) const
{
   return first_name + second_name; 
}

void employe::Print(void) const
{
    cout << FullName() << endl;
}

class manager:public employe
{
    private:
        int level;

    public:
        void SetLevel(const int l);
        void GetLevel() const;
        //void Print()const;
        manager(const string& f, const string& s, int l):employe(f, s), level(l)
    {}
};

/*
void manager::Print()const
{
    employe::Print();
    GetLevel();
}
*/

void manager::SetLevel(const int l)
{
    level = l;
}

void manager::GetLevel() const
{
    cout << level << endl;
}

class base
{
    public:
        virtual void iam()
        {
            cout << "base\n";
        }
    
        virtual ~base()
        {}    
};

class TstThrow
{
    public:
        TstThrow()
        {
            cout << "TstThrow\n";
        }
};

class child1:public base
{
    public:
        void iam()
        {
            cout << "child1\n";
        }

        ~child1()
        {
        }
};

class child2:public base
{
    public:
        void iam()
        {
            cout << "child2\n";
        }
};

template<class T, int i>
class Buff
{
    private:
        T v[i];
        int sz;

    public:
        Buff():sz(i){}

        int Size()
        {
            return sz;
        }
};

template<class T>
class String
{
    private:
        int size;
        T* var;

    public:
        String(T* a, int i)
        {
           size = i;
           var = a;  
        }

        int length()
        {
            return size;
        }

        T& operator [](int i)
        {
            return *(var+i);
        }
};

template<class T>
class Cmp
{
    public:
        static int eq(T a, T b)
        {
            return a == b;
        }

        static int lt(T a, T b)
        {
            return a < b;
        }
};

template<class T, class C=Cmp<char> >
int compare(String<T>& str1, String<T>& str2)
{
    for (int i=0; i<str1.length() && i<str2.length(); i++)
    {
        if (!C::eq(str1[i], str2[i]))
        {
            return C::lt(str1[i], str2[i])?-1:1;
        }

        return str1.length() - str2.length();
    }
}

class MathErr
{
    public:
        virtual void PrintDebug()const 
        {
            std::cout << "MathErr error.\n";
        }
};

class IntOverFlow:public MathErr
{
    private:
        char opt;
        int a1;
        int a2;

    public:
        IntOverFlow(const char p, const int a, const int b):opt(p), a1(a), a2(b){}
        void PrintDebug()const
        {
            std::cout <<"IntOverFlow error: " << opt << "(" << a1 << "," << a2 << ")" << endl;
        }
};

int Add(int a, int b)throw (MathErr);
int Add(int a, int b) throw (MathErr)
{
    if (a>0 && b>0 && a>INT_MAX-b)
    {
        throw IntOverFlow('+', a, b);
    }

    return a+b;
}

void Func(char* ptr1, char* ptr2, auto_ptr<char> ptr3)
{
    char* p = new char[10];
    auto_ptr<char> p2(ptr2);
    cout << "ptr1: " << ptr1<< endl;
}

template<class T>
class STC
{
    public:
        STC(T& val)
        {
            old = curr;
            curr = val;
        }

        virtual ~STC()
        {
            curr = old;
        }
    private:
        T curr;
        T old;
};

class Int
{
    private:
        int i;

    public:
        Int():i(0){}
        Int(int a):i(a){}
        Int& operator+(int);
};

Int& Int::operator+(int a)
{
    if (i> INT_MAX - a)
    {
        throw Error("out of int range"); 
    }
}

template<class T>
class Ptr2T
{
    private:
    T* ptr;
    T* array;
    int size;

    public:
    Ptr2T(T*p, T*a, int s):ptr(p), array(a), size(s)
    {}

    Ptr2T(T*p):ptr(p), array(p), size(1)
    {}

    T GetT()
    {
        return *ptr;
    }

    void SetT(T val)
    {
        *ptr = val;
    }
    

    Ptr2T& operator++();
    Ptr2T& operator--();
    Ptr2T operator++(int);
    Ptr2T& operator--(int);
    bool OutOfRange()
    {
        bool ret = false;
        if (ptr < array || ptr>array+size-1)
        {
            return true;
        }

        return ret;
    }
};

template<class T>
Ptr2T<T> Ptr2T<T>::operator++(int)
{
    if (ptr >= array+size-1 || ptr <= array-1)
    {
        throw Error("out of array range"); 
    }
    
    Ptr2T<T> tmp = *this;
    ptr = ptr + 1;

    return tmp;
}

template<class T> 
Ptr2T<T>&  Ptr2T<T>::operator++()
{
    if (ptr > array+size-1 || ptr <= array-1)
    {
        throw Error("out of array range"); 
    }

    ptr = ptr +1;

    return *this;
}

template<class T>
Ptr2T<T>& Ptr2T<T>::operator--()
{
    if (ptr > array+size-1 || ptr <= array)
    {
        throw Error("out of array range"); 
    }

    ptr = ptr - 1;
    return *this;
}

typedef int(*FuncPtr)(int, int) ;
class  FuncCallBack
{
    private:
    FuncPtr ptr;

    public:
    FuncCallBack():ptr(NULL){}
    FuncCallBack(FuncPtr a):ptr(a){}
    int Call(int a, int b)
    {
       return ptr(a, b);
    }
};


class StaticTst
{
    static void StaticFunc(void*pData, char* msg)
    {
        StaticTst* p = (StaticTst*)pData;
        //p->Func(msg);
    }    
};

int Sub(int a, int b)
{
    return a - b;
}

class cBaseLock
{
    public:
        virtual ~cBaseLock()
        {}

        virtual void Lock()= 0;
        virtual void UnLock()= 0;
};

class cMutex:public cBaseLock
{
    public:
        cMutex()
        {
            pthread_mutex_init(&m_mutex, NULL);
        }

        virtual ~cMutex()
        {
            pthread_mutex_destroy(&m_mutex);
        }

        void Lock()
        {
            pthread_mutex_lock(&m_mutex);
        }

        void UnLock()
        {
            pthread_mutex_unlock(&m_mutex);
        }

    private:
            pthread_mutex_t m_mutex;
};

cMutex g_mutex;
class cLock
{
    public:
        cLock(cMutex& mutex):m_cmutex(g_mutex)
        {
            m_cmutex.Lock();
        }

        ~cLock()
        {
            m_cmutex.UnLock();
        }

    private:
        cMutex& m_cmutex;
};


class Base1
{
    public:
        void Print(char a)
        {
            cout << "Base1\n";
        }
};

class Base2
{
    public:
        void Print(int a)
        {
            cout << "Base2\n";
        }
};

class Child:public Base1, public Base2
{
    public:
    using Base1::Print;
    using Base2::Print;
     void Print(int a)
     {
            cout << "Child\n";
     }
};

class Parent
{
    private:
        int p;
    public:
        Parent(int a):p(a)
    {}

    int GetInt()
    {
        return p;
    }

    ~Parent(){}

    void Wrapper()
    {
        Add();
    }

    protected:
        void Add()
        {
            p += 100;
        }
};

class ProtChild:public Parent
{
    public:
        ProtChild(int a):Parent(a){};
        virtual void ChildWrapper()
        {
            Add();
        }

        ~ProtChild(){}
};

class B
{
    private:
        int a;
    protected:
        int b;
    public:
        int c;
};

class D:private B
{
    public:
//        using B::a;
        using B::b;
        using B::c;
};

size_t ReadCallBack(void* ptr, size_t size, size_t nmem, void* userp)
{
    cout << "size: " << size <<endl;;
    cout << "ptr: "<< ptr << endl;
    return 0;
}

class cPar1
{
    private:
        char Par1[32];

    public:
        cPar1(char* ptr)
        {
            strncpy(Par1, ptr, strlen(ptr));
            Par1[strlen(ptr)] = '\0';
        }

        cPar1()
        {
            memset(Par1, 0, sizeof(Par1));
        }

        virtual void Print()
        {
            cout << "CPar1\n";
            cout << Par1 << endl;
        }

        ~cPar1(){}
};

class cPar2
{
    private:
        char Par2[32];

    public:
        cPar2(char* ptr)
        {
            strncpy(Par2, ptr, strlen(ptr));
            Par2[strlen(ptr)] = '\0';
        }

        cPar2()
        {
            memset(Par2, 0, sizeof(Par2));
        }

        virtual void Print()
        {
            cout << "Cpar2\n";
            cout << Par2 << endl;
        }
        ~cPar2(){};
};

class cChild:public cPar1, public cPar2
{
    private:
        char child[32];

    public:
        cChild(char* ptr)
        {
            strncpy(child, ptr, strlen(ptr));
            child[strlen(ptr)] = '\0';
        }

        virtual ~cChild(){};

        void Print()
        {
            cout << "Child\n";
            cout << child << endl;
        }
};

template<class T, class S>
class ptr_cast
{
    public:
        T* operator ()(S* src)
        {
            if (typeid(src) == typeid(T*))
            {
                return (T*)(src);
            }
            else
            {
                const char* ptr = "test";
                throw bad_cast();
            }
        }
};

vector< string > Split(string& str, string& split)
{
    vector<string> vStr;
    string::size_type iStart = 0;

    if (str.length() == 0)
    {
        return vStr;
    }

    string substr;
    string::size_type iPos;
    while (true)
    {
        if ((iPos=str.find(split, iStart)) == string::npos)
        {
            return vStr;
        }

        substr = str.substr(iStart, iPos - iStart);
        vStr.push_back(substr);
        iStart = iPos + split.length();
    }

    return vStr;
}

template<class T>
class MyVector
{
    private:
        T* ptr;
        size_t size;
        size_t pos;
        int ref;

    public:
        MyVector(size_t n):size(n), pos(0)
    {
        ptr = new T[n];
        ref++;
    }

        void PushBack(T val)
        {
            ptr[pos++] = val;
        }

        virtual T& operator[](int i)
        {
            return *(ptr + i);
        }

        virtual ~MyVector()
        {
            if ( ref == 0)
            {
                cout <<"in destroy MyVector\n";
                delete ptr;
                size = 0;
            }
        }

        size_t Size()
        {
            return size;
        }
};

template<class T>
class Iter
{
    public:
        Iter(){}
        ~Iter(){}

        virtual T first() = 0;
        virtual T next() = 0;
};

template<class T>
class vector_iter:public Iter<T>
{
    private:
        MyVector<T> vec;
        size_t pos;

    public:
        ~vector_iter(){};
        vector_iter(MyVector<T>& v):vec(v), pos(0){}
        T first()
        {
            if (vec.Size() == 0)
            {
                return 0;
            }

            return vec[0];
        }

        T next()
        {
            if (pos+1 >= vec.Size())
            {
                return 0;
            }

            pos++;
            return vec[pos];
        }
};

bool IsFirst(string& s)
{
    return s[0]=='s' ?true:false;
}

bool IsNotFirst(string& s)
{
    return !IsFirst(s);
}

template<class T>
bool Swap(T a, T b)
{
    T c(a);
    a = b;
    b = c;
}

template<class T, int max>
class c_array
{
    typedef T value_type;
    typedef T* iterator;
    typedef const T* const_iterator;
    typedef T& reference;
    typedef const T& const_reference;

    private:
    T array[max];

    public:
    size_t size()
    {
        return max;
    }

    iterator begin()
    {
        return array;
    }

    iterator  end()
    {
        return array + max;
    }

    operator T*()
    {
        return array;
    }

    reference operator[](int i)
    {
        return array[i];
    }
};

template <class T, class T2> 
class IsNegative:public std::unary_function<T, T2>
{
    public:
    T2 operator()(T val)
    {
        return val < 0;
    }
};

template<class T> class LessThan:public unary_function<T, bool>
{
    private:
        T arg2;
    public:
        explicit LessThan(const T& x, const T& y):arg2(x){};
        bool operator()(const T& x)const 
        {
            return x < arg2;
        }
};

class MemFuncTst
{
    public:
        void Print()
        {
            cout <<"hello";
        }
};

bool IsO(char c)
{
    return c=='o';
}

class cIsO:public unary_function<char, bool>
{
    public:
    bool operator()(char a)
    {
        return a=='o';
    }
};

int main(void)
{

    char c[] = "hello world";
    int n = count_if(c, c+strlen(c)+1, cIsO());
    cout <<"count:" << n << endl;

    /*
    vector<MemFuncTst*> vecTst;
    for (int i=0; i<10; i++)
    {
        MemFuncTst* ptr = new MemFuncTst;
        vecTst.push_back(ptr);
    }

    vector<MemFuncTst*>::iterator iter;
    for_each(vecTst.begin(), vecTst.end(), mem_fun(&MemFuncTst::Print));
    */
    /*
    list<int> listTst;
    for (int i=-10; i<10; i++)
    {
        listTst.push_back(i);
    }

    list<int>::iterator iter;
    cout <<"first list\n";
    for (iter=listTst.begin(); iter!=listTst.end(); iter++)
    {
        cout << *iter << '\t';
    }
    cout << endl;

    //iter = find_if(listTst.begin(), listTst.end(), LessThan<int>(8, 9));
    iter = find_if(listTst.begin(), listTst.end(), bind1st(less<int>(), 7)); 
    cout <<"find if is :" << *iter;
    listTst.remove_if(IsNegative<int, bool>());
    cout << "second list\n";
    for (iter=listTst.begin(); iter!=listTst.end(); iter++)
    {
        cout << *iter << '\t';
    }
    cout << endl;
    */
//    map<string, string> mapTst;
//    mapTst.insert(make_pair("hello", "world"));
//    mapTst.insert(make_pair("world", "hello"));
//    mapTst.insert(make_pair("hello", "worldxxx"));
//
    //mapTst["hello"] = "world";
    //mapTst["world"] = "hello";
    //mapTst["hello"] = "worldxxx";
    //mapTst["null"];
    /*
    map<string, string>::const_iterator iter;
    for (iter=mapTst.begin(); iter!=mapTst.end(); iter++)
    {
        cout <<"first: " << iter->first<< ",second: " << iter->second<< endl;
    }

    map<string, string> mapTst1;
    mapTst1.swap(mapTst);
    cout <<"tst" << endl;
    for (iter=mapTst.begin(); iter!=mapTst.end(); iter++)
    {
        cout <<"first: " << iter->first<< ",second: " << iter->second<< endl;
    }

    cout <<"tst1" << endl;
    for (iter=mapTst1.begin(); iter!=mapTst1.end(); iter++)
    {
        cout <<"first: " << iter->first<< ",second: " << iter->second<< endl;
    }
    return 0;
    */
    /*
    char c[20];
    vector<string> vFluat;
    int i = 0;
    while(cin>>c)
    {
        if (i++> 3)
        {
            break;
        }

        vFluat.push_back(c);
    }

    vector<string>::const_iterator constIter;
    for (constIter=vFluat.begin(); constIter!=vFluat.end(); constIter++)
    {
        cout << *constIter << '\t';
    }
    cout << endl;

    sort(vFluat.begin(), vFluat.end());
    for (constIter=vFluat.begin(); constIter!=vFluat.end(); constIter++)
    {
        cout << *constIter<< '\t';
    }
    cout << endl;
*/


    /*
    vector<char> vChar;
    for (char i='z'; i>='a'; i--)
    {
        vChar.push_back(i);
    }

    for (char i='Z'; i>='A'; i--)
    {
        vChar.push_back(i);
    }

    int i = 0;
    vector<char>::const_iterator iter;
    */
    /*
    for (iter=vChar.begin(); iter!=vChar.end(); iter++)
    {
        cout <<*iter;
        cout << "  ";
        if (++i%10 == 0)
        {
            cout << "\n";
        }
    } 
    cout << "\n";
    */
    
    /*
    sort(vChar.begin(), vChar.end());
    for (iter=vChar.begin(); iter!=vChar.end(); iter++)
    {
        cout <<*iter;
        cout << "  ";
        if (++i%10 == 0)
        {
            cout << "\n";
        }
    } 
    cout << "\n";

    vector<char>::const_reverse_iterator reiter;
    for (reiter=vChar.rbegin(); reiter!=vChar.rend(); reiter++)
    {
        cout <<*reiter;
        cout << "  ";
        if (++i%10 == 0)
        {
            cout << "\n";
        }
    }
    cout << "\n";
    */
    /*
    vector<string> vecStr;
    vecStr.push_back("string1");
    vecStr.push_back("str2");
    vecStr.push_back("int1");
    vecStr.push_back("int2");
    vecStr.push_back("str3");
    vector<string>::iterator iter = vecStr.begin();
    for(iter=vecStr.begin(); iter<vecStr.end(); iter++)
    {
        cout << *iter << endl;
    }
    
    vecStr.erase(vecStr.begin());
    cout << "****************************\n";
    for(iter=vecStr.begin(); iter<vecStr.end(); iter++)
    {
        cout << *iter << endl;
    }
    return 0;
     
    sort(vecStr.begin(), vecStr.end());
    vector<string>::iterator iter_beg = find_if(vecStr.begin(), vecStr.end(), IsFirst); 
    vector<string>::iterator iter_end = find_if(iter_beg, vecStr.end(), IsNotFirst);
    vecStr.erase(iter_beg, iter_end);
    
    cout << "****************************\n";
    for(iter=vecStr.begin(); iter<vecStr.end(); iter++)
    {
        cout << *iter << endl;
    }
*/
    /*
    vector<int> vecInt;
    for (int i=0; i<100; i++)
    {
        vecInt.push_back(i);
    }

    vector<int>::iterator iter = vecInt.begin();

    int i=0;
    while (iter++ != vecInt.end())
    {
        cout << *iter ;
        cout <<" ";
        if (++i%10 == 0)
        {
            cout << endl;
        }
    }

    cout << endl;
    return 0;
    */

    /*
    MyVector<int> vec(10);
    for (int i=0; i<10; i++)
    {
        vec.PushBack(i+1);
    }

    vector_iter<int> iter(vec); 
    cout <<"first:" << iter.first() << endl;
    */

    /*
    string str = "hello world hello china, hello jiangxi, hello ganzhou, hello.";
    string split = "hello";

    vector<string> vStr = Split(str, split);

    vector<string>::iterator iter = vStr.begin();
    for (; iter!=vStr.end(); iter++)
    {
        cout << *iter << endl;
    }

    return 0;
   */ 
    /*
        try
        {
            cChild* ptr =  new cChild("hello world");
            ptr->cPar1::Print();
            ptr_cast<cChild, cChild> objCast;
            cChild* cptr = objCast(ptr);
        } 

        catch (bad_cast& e)
        {
            cout << "catch bad_cast:" << e.what() << endl;
        }
        catch(...)
        {
            cout << "catch other cast\n";
        }
    */
//    cPar1* ptr =  new cChild("hello world");
 //   ptr->cPar1::Print();

    /*
    CURL* curl;
    CURLcode res;
    res = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (res != CURLE_OK)
    {
        cout << "curl_global_init failed.\n";
        return 0;
    }
    curl =curl_easy_init();
    if (!curl)
    {
        cout << "curl_easy_init failed.\n";
        return 0;
    }

    curl_easy_setopt(curl, CURLOPT_URL, "http://api.weixin.oa.com/itilalarmcgi/sendmsg"); 
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, ReadCallBack);
    curl_easy_setopt(curl,CURLOPT_POSTFIELDS, "Sender=v_monitor&Rcptto=willrlzhang&isText=curlhelloworld");

    res = curl_easy_perform(curl); 
    if (res != CURLE_OK)
    {
        cout << "curl_easy_perform failed.\n";
        return 0;
    }

    curl_easy_cleanup(curl);
    return 0;
    */
    /*
    ProtChild child(10);
    Parent* ptr = dynamic_cast<Parent*>(&child);  
    child.ChildWrapper();
    int i = ptr->GetInt();
    cout << "i: " << i<< endl;


    return 0;
    Child ch;
    ch.Print(1);
    string msg="hello world";
    setStrAttrWithIP("172.27.10.73", 2000000, msg.size(), (char*)msg.c_str());
    return 0;
    */

    /*
    try
    {

        FuncCallBack obj(Sub);
        int ret = obj.Call(10, 100);
        cout << "FuncCallBack result: " << ret << endl;
    Int t(INT_MAX);
    t+1;

    int a[100];

    Ptr2T<int> exam(&a[0], a, 100);
    
    for (int i=0; i< 99 ; i++)
    {
        exam.SetT(i);
        ++exam;
    }

    --exam;
    int a1 = exam.GetT();
    --exam;
    int a2 = exam.GetT();
    cout << "a1=" << a1 << "a2=" << a2 << endl;
    }
    catch (runtime_error& e)
    {
        cout <<"catch error.\n";
    }
    catch (...)
    {
        cout << "other error.\n";
    }
    Buff<char, 10> cBuf;
    cout << "size is " << cBuf.Size() << endl;
    char buf1[] = "hello";
    char buf2[] = "world";
    String<char> str1(buf1, 5);
    String<char> str2(buf2, 5);
    
    int ret = compare(str1, str2);
    cout << "ret is " << ret << endl;

    char *ptr1 = new char[10];
    strncpy(ptr1, "hello", 10);
    cout <<"ptr1" << ptr1 << endl;

    char *ptr2 = new char[10];
    strncpy(ptr2, "world", 10);
    cout <<"ptr2" << ptr2 << endl;

    char *ptr4 = new char[10];
    strncpy(ptr4, "xxxx", 10);
    cout <<"ptr4" << ptr4 << endl;

    auto_ptr<char> ptr3(ptr4);
    Func(ptr1, ptr2, ptr3);

    cout <<"ptr1" << ptr1 << endl;
    cout <<"ptr2" << ptr2 << endl;
    cout <<"ptr4" << ptr4 << endl;

    try{
        Add(1,1);
        Add(INT_MAX, 10);
    }
    catch (MathErr &err)
    {
        err.PrintDebug();
    }
    catch (...)
    {
        std::cout << "other catch ....\n";
    }

    child1 c1;
    child2 c2;
    base* ptr1 = &c1;
    base* ptr2 = &c2;
    ptr1->iam();
    ptr2->iam();
    employe exam("green", "jim");
    exam.Print();
    employe exap1 = exam;
    exap1.Print();

    manager mang("li", "lei", 100);
    employe* ptr = &mang;
    mang.Print();
    */

    return 0;
}

