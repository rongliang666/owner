#include <stddef.h>
#include <iostream>

typedef struct sOffset
{
    int a;
    char b[10];
    char *ptr;
}SOFFSET;

//only use c struct, can't use class
#define OFFSETOF(st, m) ((size_t)(&(((st* )0)->m)))

int main(void)
{
    SOFFSET st;
    size_t off = OFFSETOF(SOFFSET, a);
    std::cout <<"a offset: " <<  off << std::endl;
    off = OFFSETOF(SOFFSET, b) ;
    std::cout <<"b offset: " << off << std::endl;
    off =OFFSETOF(SOFFSET, ptr) ;
    std::cout <<"ptr offset: " << off << std::endl;

}
