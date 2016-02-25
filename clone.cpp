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
};

class cDerived:public cBase
{
    public:
        cDerived* Clone()const
        {
            std::cout <<"cDerived clone\n";
            return new cDerived(*this);
        }
};

void Copy(cBase& c)
{
    c.Clone();
}

int main(void)
{
    cBase b;
    cDerived dervied;
    Copy(b);
    Copy(dervied);
}
