#include <iostream>

class cFriend
{
    public:
        void Display(void)
        {
            std::cout << m_sName << std::endl;
        }

        void SetName(std::string& sStr)
        {
            if (!m_sName.empty())
            {
                m_sName +=" ";
            }
                m_sName += sStr;
        }

        void Covert()
        {
            size_t pos = 0;
            pos = m_sName.find_first_of(m_sSplit);
            while (pos != std::string::npos)
            {
                m_sName[pos] = 'z';
                pos = m_sName.find_first_of(m_sSplit, pos+1);
            }

        }

        void SetSplit(const std::string& s)
        {
           m_sSplit = s; 
        }

    private:
        std::string m_sName;
        std::string m_sSplit;

};

int main(void)
{
    std::string s;
    int i = 0;
    cFriend f;
    f.SetSplit("aeiouw");
    while (i<2)
    {
        i++;
        std::getline(std::cin, s);
        f.SetName(s);
    }

    f.Display();
    f.Covert();
    std::cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n";
    f.Display();

    return 0;
}

