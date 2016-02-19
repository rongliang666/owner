#include<stdlib.h>
#include <iostream>
#include <time.h>

//random [min, max]
int FunRandom1(int min, int max)
{
    srand(time(NULL));
    int ret = rand()%(max-min+1) + min;
    return ret;
}

//random [min, max)
int FunRandom2(int min, int max)
{
    srand(time(NULL));
    int ret = rand()%(max-min) + min;
    return ret;
}

//random (min, max)
int FunRandom3(int min, int max)
{
    return FunRandom1(min-1, max-1);
}

//random (min, max]
int FunRandom4(int min, int max)
{
    srand(time(NULL));
    int ret = rand()%(max-min) + min +1;
    return ret;
}

//random float 0-1
float FunRandom5()
{
    srand(time(NULL));
    float ret = (float)rand()/RAND_MAX;
    return ret;
}

int main()
{
    int min = 10;
    int max = 10000000;

    int ret = FunRandom1(min, max);
    std::cout << "FunRandom1\t" << ret << std::endl;
    ret = FunRandom2(min, max);
    std::cout << "FunRandom2\t" << ret << std::endl;
    ret = FunRandom3(min, max);
    std::cout << "FunRandom3\t" << ret << std::endl;
    ret = FunRandom4(min, max);
    std::cout << "FunRandom4\t" << ret << std::endl;
    float t = FunRandom5();
    std::cout << "FunRandom5\t" << t << std::endl;
}
