#include <iostream>
#include <array>

using namespace std;

int main(void)
{
    array<int, 5> a = {1, 2, 3, 4, 5};

    array<int, 5>::iterator iter;
    int i = 0;
    for (auto it=a.begin(); it!=a.end(); it++)
    {
        cout <<"element " << i++ << "\t:" <<  *it;
    }

    cout << endl;
    return 0;
}
