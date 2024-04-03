#include "allocator.h"
#include <map>
#include <memory>

template<typename T, typename Allocator = std::allocator<T>>
class Container {
public:
    Container() {}
    
    ~Container() {
         std::allocator_traits<decltype(alloc_)>::destroy(alloc_, data_);
         std::allocator_traits<decltype(alloc_)>::deallocate(alloc_, data_, CAPACITY);
    }

    void push_back(const T& item) {
        if (!data_) {            
            size_ = 0;
            data_ = std::allocator_traits<decltype(alloc_)>::allocate(alloc_, CAPACITY);
        }

        std::allocator_traits<decltype(alloc_)>::construct(alloc_, data_ + size_, item);
        size_++;
    }

    std::size_t size() const {
        return size_;
    }
    
    const T& at(int idx) const {
        if (0 <= idx && idx < (int)size_) {
            return data_[idx];
        }
        throw std::out_of_range("out_of_range");
    }

private:
    static constexpr std::size_t CAPACITY = 10;

    Allocator alloc_;
    std::size_t size_ = 0;
    T* data_ = nullptr;
};


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
    {
        std::map<int, int> data;
        for (int i = 0; i < 10; i++) {
            data[i] = factorial(i);
        }
    }

    {
        std::map<int, int, std::less<int>, pp::Allocator<std::pair<int, int>>> data;
        for (int i = 0; i < 10; i++) {
            data[i] = factorial(i);
        }

        for (const auto& [k, v] : data) {
            std::cout << k << ' ' << v << std::endl;
        }
    }
    
    {
        Container<int> data;
        for (int i = 0; i < 10; i++) {
            data.push_back(i);
        }
    }

    {
        Container<int, pp::Allocator<int>> data;
        for (std::size_t i = 0; i < 10; i++) {
            data.push_back(i);
        }
        
        for (std::size_t i = 0; i < data.size(); i++) {
            std::cout << data.at(i) << std::endl;
        }
    }
}