#include <iostream>
#include <vector>
#include <sstream>
#include <stdint.h>

class cFibonacii
{
    public:
        cFibonacii(int size):m_size(size)
    {}

        void DoFibonacii()
        {
            m_vecData.push_back(0);
            m_vecData.push_back(1);

            for (int i=2; i<m_size; i++)
            {
                m_vecData.push_back(m_vecData[i-1]+m_vecData[i-2]);
            }
        }

        void DisPlay()
        {
            for (int i=0; i< m_size;) 
            {
                std::cout << m_vecData[i] << "\t";
                if (++i%10 == 0)
                {
                    std::cout << "\n";
                }
            }

            std::cout << "\n";
        }

    private:
        int m_size;
        std::vector<int64_t> m_vecData;
};

template<class TI, class TO>
TO ToType(TI a)
{
    std::stringstream s;
    TO b;
    s << a;
    s >> b;

    return b;
}

int main()
{

    std::string sData;
    std::getline(std::cin, sData); 
    int i = ToType<std::string, int>(sData);
    std::cout <<"input " << i << std::endl;

    cFibonacii obj(i);
    obj.DoFibonacii();
    obj.DisPlay();
}
