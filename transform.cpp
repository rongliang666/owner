#include <iostream>
#include <functional>
#include <ctype.h>
#include <algorithm>

using namespace std;
int main(void)
{
    string s("hello world\n");
    cout << s;
    transform(s.begin(), s.end(), s.begin(), ptr_fun<int, int>(toupper));
    cout << s;
}
