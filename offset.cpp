#include <stddef.h>
#include <string.h>
#include <iostream>

typedef struct sOffset
{
    int a;
    char b[10];
    char *ptr;
}SOFFSET;

//only use c struct, can't use class
#define OFFSETOF(st, m) ((size_t)(&(((st* )0)->m)))
#define container_of(ptr, type, member) ({\
       typeof(((type* )0)->member)* __mptr=(ptr);\
        (type* )((char* )__mptr-OFFSETOF(type, member));\
        })

int main(void)
{
    SOFFSET st;
    st.a = 10;
    strncpy(st.b, "hello world", sizeof(st.b));
    st.ptr = NULL;
    size_t off = OFFSETOF(SOFFSET, a);
    std::cout <<"a offset: " <<  off << std::endl;
    off = OFFSETOF(SOFFSET, b) ;
    std::cout <<"b offset: " << off << std::endl;
    off =OFFSETOF(SOFFSET, ptr) ;
    std::cout <<"ptr offset: " << off << std::endl;

    SOFFSET* ptr;
    ptr = container_of(&(st.a), SOFFSET, a);
    std::cout <<"container_of ptr->b" << ptr->b << std::endl;
}
