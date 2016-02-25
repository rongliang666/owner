#include <iostream>

class cClone
{
    public:
        virtual ~cClone()
        {}

        virtual cClone* Clone()const=0;
};

class cBase:public cClone
{
    public:
        cBase* Clone()const
        {
            std::cout <<"clone cBase\n";
            return new cBase(*this);
        }

        virtual ~cBase()
        {
            std::cout <<"destroy cBase\n";
        }
};

class cDerived:public cBase
{
    public:
        cDerived* Clone()const
        {
            std::cout <<"cDerived clone\n";
            return new cDerived(*this);
        }

        virtual ~cDerived()
        {
            std::cout << "destroy cDerived\n";
        }
};

void Copy(cBase& c)
{
    cBase* ptr = c.Clone();
    delete ptr;
}

int main(void)
{
    cBase b;
    std::cout <<"***********************************\n";
    cDerived dervied;
    std::cout <<"***********************************\n";
    Copy(b);
    std::cout <<"***********************************\n";
    Copy(dervied);
    std::cout <<"***********************************\n";
}
