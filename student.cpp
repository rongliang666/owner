#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

class cStudent
{
    public:
        cStudent(const std::string& firname, const std::string secname, int id):
            m_sFirstName(firname), m_sSecName(secname), m_iId(id)
        {}

        std::string GetFirst() const
        {
            return m_sFirstName;
        }

        std::string GetSec() const
        {
            return m_sSecName;
        }

        int GetId() const 
        {
            return m_iId;
        }

       friend std::ostream& operator <<(std::ostream& os, const cStudent& stu)
       {
           os << stu.GetFirst() << "\t" << stu.GetSec() <<"\t" << stu.GetId() << std::endl;
           return os;
       }

       bool operator <(const cStudent& stu)
       {
           return m_iId < stu.m_iId;
       }
      
    private:
        std::string m_sFirstName;
        std::string m_sSecName;
        int m_iId;
};

struct  EachFunc
{
    void operator ()(const cStudent& s)
    {
        std::cout << s;
    }
}Func;

struct Count10000
{
   bool operator ()(const cStudent& s)
   {
       return s.GetId() <10000;
   } 
}C10000;

struct Count10
{
    bool operator ()(const cStudent& s)
    {
        return s.GetId() < 10;
    }
}C10;

class CountPre:public std::binary_function<cStudent, int ,bool>
{
    public:
    bool operator()(cStudent& s, int sec) const
    {
        return s.GetId() < sec;
    }
};

bool FuncCount(const cStudent s, int i)
{
    return s.GetId()<i;
}

bool test(int i, int b)
{
    std::cout <<"first " << i << "\tsecond" << b << std::endl;
    return i < b;
}

int main()
{
    std::vector<cStudent> vecStus;
    cStudent stu1("zhang", "san", 100);
    cStudent stu2("li", "si", 1000);
    cStudent stu3("zhao", "wu", 1);
    vecStus.push_back(stu1);
    vecStus.push_back(stu2);
    vecStus.push_back(stu3);

    std::for_each(vecStus.begin(), vecStus.end(), Func);

    int count = 0;// =  std::count(vecStus.begin(), vecStus.end(), 0) ;

    count = std::count_if(vecStus.begin(), vecStus.end(), std::bind2nd(CountPre(), 10000));
    std::cout << "Count10000 : " << count << std::endl;;

    count = std::count_if(vecStus.begin(), vecStus.end(), std::bind2nd(CountPre(), 10));
    std::cout << "Count10: " << count << std::endl;;

    count = std::count_if(vecStus.begin(), vecStus.end(), std::bind2nd(CountPre(), 1000));
    std::cout << "Count100: " << count << std::endl;;

    //note, FuncCount ,the argv can't have '&',or ths ptr_fun covent will failed.
    //this scene use functor
    count = std::count_if(vecStus.begin(), vecStus.end(), std::bind2nd(std::ptr_fun(FuncCount), 100));
    std::cout << "Count FuncCount: " << count << std::endl;;

    //count = std::count_if(vecStus.begin(), vecStus.end(), std::bind2nd(std::pointer_to_binary_function<cStudent, int, bool(*)(const cStudent&, int)>(FuncCount), 100));
//    std::bind2nd(std::ptr_fun(test), 10)(100);

    return 0;

}
