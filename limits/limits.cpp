#include <limits>
#include <iostream>

const double eps = std::numeric_limits<double>::epsilon();

int main(int argc, char *argv[])
{
    std::cout << "1.0 + eps > 1.0 is " << (1.0 + eps > 1.0) << std::endl;
    std::cout << "1.0 + eps / 2 > 1.0 is " << (1.0 + 0.5 * eps > 1.0) << std::endl;
    return 0;
};
