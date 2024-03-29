#include "allocator.h"
#include <map>
#include <cassert>

int factorial(int num) {
    assert(num >= 0);

    if (num == 0) {
        return 1;
    }

    int res = num;
    while (num != 1) {
        res *= (num - 1);
        num--;
    }

    return res;
}

int main() {        
    std::map<int, int> data1;
    for (int i = 0; i < 10; i++) {
        data1[i] = factorial(i);
    }

    std::map<int, int, std::less<int>, Allocator<std::pair<int, int>>> data2;
    for (int i = 0; i < 10; i++) {
        data2[i] = factorial(i);
    }

    for (const auto& [k, v] : data2) {
        std::cout << k << ' ' << v << std::endl;
    }
    
    
       

}