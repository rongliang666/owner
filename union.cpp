#include <iostream>

using namespace std;

//not use like this
typedef struct stChar
{
    unsigned char char0;
    unsigned char char1;
    unsigned char char2;
    unsigned char char3;
}Chart_t;

typedef union 
{
    unsigned int i;
    Chart_t c;
}Packed_t;

int main(void)
{
    Packed_t v;
    v.i =  3232235UL;
    cout << v.c.char0 << ", " << v.c.char1 <<", " << v.c.char2<< ", " <<v.c.char3 << endl;
    cout << "int " << v.i;

    v.c.char0 = 10;
    v.c.char1 = 13;
    v.c.char2 = 105;
    v.c.char3 = 71;
    cout << v.c.char0 << ", " << v.c.char1 <<", " << v.c.char2<< ", " <<v.c.char3 << endl;
    cout << "int " << v.i;

}
