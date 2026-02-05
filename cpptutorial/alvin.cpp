#include <iostream>
#include <vector>
#include <string>
#include <random>

bool J = -0.5;

class randomNumber
{
private:
    int lower;
    int upper;
    std::mt19937 gen;
    std::uniform_int_distribution<int> distr;

public:
    randomNumber(int lowerArg, int upperArg)
        : lower(lowerArg), upper(upperArg), gen(std::random_device{}()), distr(lowerArg, upperArg)
    {
    }
};

int getRandomInt(int a, int b)
{
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> distr(a, b);
    return distr(gen);
}

class Matrix
{
private:
    int size;
    std::vector<std::vector<int>> gridValue;

public:
    Matrix(int sizeArg)
    {
        size = sizeArg;
        gridValue.resize(size, std::vector<int>(size, 0));
    }

    void print()
    {
        for (int m = 0; m < size; m++)
        {
            for (int n = 0; n < size; n++)
            {
                std::cout << gridValue[n][m] << " ";
            }
            std::cout << "\n";
        }
    }

    void makeRandom()
    {
        for (int m = 0; m < size; m++)
        {
            for (int n = 0; n < size; n++)
            {
                gridValue[n][m] = getRandomInt(0, 1);
                if (gridValue[n][m] == 0)
                {
                    gridValue[n][m] = -1;
                }
            }
        }
    }
};

int main()
{
    std::cout << "Random number between 1 and 10: " << getRandomInt(1, 10) << "\n";
    Matrix myMatrix(7);
    myMatrix.print();

    std::cout << "and now the Matrix has been printed.\n";

    myMatrix.makeRandom();
    myMatrix.print();
    std::cout << "and now the Matrix has been randomized and printed again, but random.\n";
    return 0;
}