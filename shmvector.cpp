#include <iostream>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <string>

using namespace boost::interprocess;
typedef allocator<int, managed_shared_memory::segment_manager>   ShmemAllocator;
typedef vector<int, ShmemAllocator> MyVector;

int main(int argc, char** argv)
{
    if (argc == 1)
    {

     struct shm_remove
        {
            shm_remove() { shared_memory_object::remove("MySharedMemory"); }
            ~shm_remove() {shared_memory_object::remove("MySharedMemory");}
        }remover;

    managed_shared_memory segment(create_only, "MySharedMemory", 65536); 
    const ShmemAllocator alloc_inst (segment.get_segment_manager());
    MyVector *myvector = segment.construct<MyVector>("MyVector")(alloc_inst);
    for(int i = 0; i < 100; ++i) 
    {
        myvector->push_back(i);
    }

    std::cout <<"myvector size is " << myvector->size() << std::endl;
     std::string s(argv[0]); s += " child ";
     if(0 != std::system(s.c_str()))
     {
         std::cout <<"system failed.\n";
         return 1;
     }

     if(segment.find<MyVector>("MyVector").first)
     {
         return 1;
     }
    }
    else
    {
        managed_shared_memory segment(open_only, "MySharedMemory");
        MyVector *myvector = segment.find<MyVector>("MyVector").first;
        int num = myvector->size();
        std::cout <<"child myvector size is " << num << std::endl;
     std::string s(argv[0]); s += " child ";
        //std::sort(myvector->rbegin(), myvector->rend());
        for(int i = 0; i < num; i++)
        {
            std::cout <<"i:" << myvector->back() << "\n";
            myvector->pop_back();
        }

        segment.destroy<MyVector>("MyVector");
    }

    return 0;
}

