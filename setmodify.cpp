#include<iostream>
#include<set>
#include <iterator>
#include <algorithm>
#include <functional>

using namespace std;
class test
{
    public:
        test(int k, int v):key(k), value(v)
    {}

        bool operator<(const test& t)const 
        {
            return this->key < t.GetKey();
        }

        void setValue(int v)
        {
            value = v;
        }

        int GetKey()const
        {
            return key;
        }

        friend ostream& operator<<(ostream& out, test& t)
        {
            cout << "key:" << t.key <<"\tvalue:" << t.value;
        }

        void Print(void) const
        {
            cout << "key:" << key <<"\tvalue:" << value <<"\n";
        }

    private:
        int key;
        int value;
};

int main(void)
{
    set<test> setTest;
    for (int i=0; i<10; i++)
    {
        setTest.insert(test(i, i*10));
    }

    for_each(setTest.begin(), setTest.end(), mem_fun_ref(&test::Print));
    set<test>::iterator it = setTest.find(test(5, 0));
    if (it != setTest.end())
    {
        //static_cast<test>(*it).setValue(5000); it's wrong only set temp obj
        test t(*it);
        setTest.erase(it); 
        t.setValue(5000);

        setTest.insert(t);
    }

    std::cout <<".............................\n";
    for_each(setTest.begin(), setTest.end(), mem_fun_ref(&test::Print));
    return 0;
}

