#include <iostream>
#include <vector>
#include <stdlib.h>
#include <algorithm>

class cCompare
{
    public:
        bool operator()(int a, int b)
        {
            return a>b;
        }

};

void PrintV(std::vector<int>& v)
{
    std::cout <<"**********************************\n";
    for (int i=0;i<v.size(); i++)
    {
        std::cout <<v[i] << "\t";
    }
     std::cout << std::endl;

}

int main()
{
    std::vector<int> v;
    srand(time(NULL));
    for (int i=0; i<10; i++)
    {
        int var = rand()%10000;
        v.push_back(var);
    }
    
    PrintV(v);
    //std::sort(v.begin(), v.end(), std::greater<int>());
    std::sort(v.begin(), v.end(), cCompare());
    PrintV(v);

}
