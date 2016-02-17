#include <iostream>
#include <vector>
using namespace std;

int main(void)
{
	cout << "hello world\n";
	vector<int> vecT;
	for (int i=0; i<10; i++)
	{
		vecT.push_back(i);
	}
	
	vector<int>::iterator iter;
	for (iter=vecT.begin(); iter!=vecT.end(); iter++)
	{
		cout <<*iter << '\t';
		
	}
	
	cout << endl;
		
	return 0;
}

