#include<iostream>
#include<algorithm>
#include<set>
#include<iterator>
using namespace std;

class StringPtrLess//:public binary_function<const string*, const string*, bool>
{
    public:
    template<class T>
    bool operator()(const T* ptr1, const T* ptr2) const
    {
        return *ptr1 < *ptr2;
    }
};

class Print
{
    public:
        void operator()(const string* str)
        {
            cout << *str << endl;
        }
};

class Deference
{
    public:
            template<class T>
            const T& operator()(T* ptr)const
            {
                return *ptr;
            }
};

int main(void)
{
    set<string*, StringPtrLess> setStr;
    setStr.insert(new string("ing"));
    setStr.insert(new string("abc"));
    setStr.insert(new string("def"));
    
    for_each(setStr.begin(), setStr.end(), Print());
    transform(setStr.begin(), setStr.end(), ostream_iterator<string>(cout ,"\n"), Deference());
    return 0;
    for (set<string*>::iterator it=setStr.begin(); it!=setStr.end(); it++)
    {
        cout << **it<< endl;
    }

    return true;
}

