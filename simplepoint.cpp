#include <iostream>

void populatearray(int** ptr, int* count)
{
    int a[] = {1, 2, 3, 4, 5};

    int* p= new int[5];
    for (int i=0; i<5; i++)
    {
        p[i] = a[i];
    }

    *ptr = p;
    *count = 5;
}

//error, no delete[]:
int main()
{
    int count;
    int* ptr;

    populatearray(&ptr, &count);
    for (int i=0; i<count; i++)
    {
        std::cout << ptr[i] << "\t";
    }
    std::cout <<std::endl;

    return 0;
}
