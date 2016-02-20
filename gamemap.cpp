#include<iostream>

enum DIR 
{
    EAEST = 1,
    SOURTH,
    WEST,
    NORTH
};

class cGameMap
{
    public:
        cGameMap(int x=0, int y=0):m_pPos(x, y)
    {}

        cGameMap(std::pair<int, int> p):m_pPos(p)
    {}

        void SetX(int x)
        {
            m_pPos.first = x;
        }

        void SetY(int y)
        {
            m_pPos.second = y;
        }

        void SetPos(int x, int y)
        {
            m_pPos.first = x;
            m_pPos.second = y;
        }

        void SetPos(std::pair<int, int>& p)
        {
            m_pPos = p;
        }

        std::pair<int, int> GetPos(void)
        {
            return m_pPos;
        }

        int GetX()
        {
            return m_pPos.first;
        }

        int GetY()
        {
            return m_pPos.second;
        }

        
        void Move(DIR i, int step)
        {
            switch (i)
            {
                case EAEST:
                    m_pPos.first += step;
                    break;

                case SOURTH:
                    m_pPos.second -= step;
                    break;

                case WEST:
                    m_pPos.first -= step;
                    break;

                case NORTH:
                    m_pPos.second += step;
                    break;

                default:
                    std::cout << "error direction\n";
            }
        }

        void PrintPos()
        {
            std::cout<<"now pos. x=" << m_pPos.first << "\ty=" << m_pPos.second << std::endl;
        }

    private:
        std::pair<int, int> m_pPos;
};

int main()
{
    cGameMap game;
    game.PrintPos();
    game.Move(EAEST, 10);
    game.PrintPos();
    game.Move(NORTH, 10);
    game.PrintPos();

    return 0;
}


