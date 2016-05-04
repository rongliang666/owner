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

bool IsTrue(int i)
{
    return i<100;
}

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
        int var = rand()%200;
        v.push_back(var);
    }
    
    PrintV(v);
    //std::sort(v.begin(), v.end(), std::greater<int>());
    //std::partial_sort(v.begin(), v.begin()+4, v.end(), cCompare());
    //std::nth_element(v.begin(), v.begin()+4, v.end(), cCompare());
     
    std::vector<int>::iterator ret = remove_if(v.begin(), v.end(), IsTrue);
    std::vector<int>::iterator  it = v.begin();
    for (;it!=ret; ++it)
    {
        std::cout <<*it << '\t';
    }

    std::cout <<'\n';
    PrintV(v);

}
