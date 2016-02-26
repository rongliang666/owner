#include <iostream>

class cInput
{
    public:
        cInput(int a, int b):m_a(a), m_b(b)
    {}
        cInput()
        {}


        friend std::istream& operator>>(std::istream& s, cInput& put)
        {
            char c = 0;
            int a;
            int b;

            s >> c;
            if (c == '(')
            {
                s >> a >> c;
                if (c == ',')
                {
                    s >> b >> c;
                    if (c == ')')
                    {
                        //put = cInput(a, b);
                    }
                }
            }
            else
            {
                s.clear(std::ios_base::failbit);
            }
            
            if (s) put = cInput(a, b);

            return s;
        }

        void Display()
        {
            std::cout <<'(' << m_a << ',' << m_b << ')' << std::endl;
        }

    private:
        int m_a;
        int m_b;
};

int main(void)
{
    cInput c;
    std::cin >> c;
    c.Display();

}
