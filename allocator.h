#pragma once

#include <iostream>
#include <cstring>
#include <cassert>

namespace pp {

template<typename T>
class Allocator {
public:
    using value_type = T;
    static constexpr std::size_t SIZE = 10;

    Allocator() noexcept {}

    template<typename U>
    Allocator(const Allocator<U>&) noexcept {}

    T* allocate(std::size_t n) {
        assert(n <= SIZE);

        T* addr;
        if (data_) {
            if ((allocatedCount_ + n) > SIZE) {
                throw std::out_of_range("out_of_range");
            }
            addr = data_ + allocatedCount_;
            allocatedCount_ += n;
        } else {
            allocatedCount_ = n;
            deallocatedCount_ = 0;
            //std::cout << "try to allocate " << (sizeof(T) * SIZE) << " bytes" << std::endl;
            data_ = static_cast<T*>(::operator new(sizeof(T) * SIZE));
            addr = data_;
        }

        //std::cout << "allocated " << n << " items" << std::endl;
        return addr;
    }

    void deallocate(T*, std::size_t n) {
        //std::cout << "deallocate " << n << " items" << std::endl;

        if (data_ && allocatedCount_ > 0) {
            if ((deallocatedCount_ + n) >= allocatedCount_) {
                ::operator delete(data_);
                data_ = nullptr;
                //std::cout << "free memory" << std::endl;
            }
            deallocatedCount_ += n;
        }
    }

private:
    T* data_ = nullptr;
    std::size_t allocatedCount_ = 0;
    std::size_t deallocatedCount_ = 0;
};

template<typename T, typename U>
constexpr bool operator== (const Allocator<T>& lhs, const Allocator<U>& rhs) noexcept {
    return lhs.data_ == rhs.data_;
}

template<typename T, typename U>
constexpr bool operator!= (const Allocator<T>& lhs, const Allocator<U>& rhs) noexcept {
    return lhs.data_ != rhs.data_;
}

} //namespace pp