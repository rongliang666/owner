#include <iostream>
#include <fstream>

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "wrong argc,  exe infile outfile\n";
        return 0;
    }

    std::ifstream in(argv[1]);
    if (!in)
    {
        std::cout <<"in stream open file failed.:" << argv[1] << std::endl;
        return 0;
    }

    std::ofstream out(argv[2]);
    if (!out)
    {
        std::cout <<"out stream open file failed.:" <<argv[2] << std::endl;
        return 0;
    }

    char c= 0;
    while (in.get(c))
    {
        out.put(c);
    }

    in.close();
    out.close();
    return 0;
}
