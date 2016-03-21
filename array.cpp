#include <iostream>
#include <array>
#include "comm_tool.h"

using namespace std;

int main(int argc, char** argv)
{
    const int size =  10000;
    array<int, size> a;
    a.fill(1);

    array<int, size> b;
    b.fill(100);
    CTimeCost timec;
    a.swap(b);
    cout << "time :" << timec.value() << endl;

    vector<int> veca(size, 1);
    vector<int> vecb(size, 100);

    CTimeCost timevec;
    veca.swap(vecb);
    cout << "size is : " << size << "\ttime :" << timevec.value() << endl;

    array<int, 5>::iterator iter;
    int i = 0;
    for (auto it=a.begin(); it!=a.end(), i<5; it++)
    {
        cout <<"element " << i++ << " :" <<  *it << "\t";
    }

    cout << endl;
    return 0;
}
