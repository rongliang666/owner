#include<iostream>
#include <iterator>
#include <vector>
#include<algorithm>
#include<functional>

using namespace std;

int main(void)
{
    vector<int> v;
    for (int i=0;i<100;i++)
    {
        v.push_back(i);
    }

    vector<int>::const_iterator constiter = find(v.begin(), v.end(), 40);
    vector<int>::iterator iterx = v.begin() ; 
    vector<int>::iterator iter = v.begin();
    advance(iter, distance<vector<int>::const_iterator>(iterx, constiter));
    cout << *iter << endl;
    
    //if (iter == constiter)
    {
        cout <<"equal\n";
    }
   // else
    {
        cout << "not equal \n";
    }
}

