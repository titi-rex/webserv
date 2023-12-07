#include <tbb/tbb.h>
#include <iostream>

int main() {
    tbb::parallel_for(0, 10, [](int i) {
        std::cout << i << " ";
    });
    std::cout << std::endl;

    return 0;
}
