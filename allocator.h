#pragma once

#include <iostream>
#include <cstring>

template<typename T>
class Allocator {
public:
    using value_type = T;
    static constexpr std::size_t SIZE = 10;

    Allocator() noexcept {}

    template<typename U>
    Allocator(const Allocator<U>&) noexcept {}

    T* allocate(std::size_t n) {
        if (data_) {
            if (allocatedCount_ == capacity_) {
                //std::cout << "try to reallocate " << (sizeof(T) * (capacity_ * 2)) << " bytes" << std::endl;
                auto ptr = static_cast<T*>(::operator new(sizeof(T) * (capacity_ * 2)));
                std::memcpy(ptr, data_, sizeof(T) * capacity_);
                ::operator delete(data_);
                data_ = ptr;
                capacity_ *= 2;
            }
            allocatedCount_++;
        } else {
            allocatedCount_ = 1;
            deallocatedCount_ = 0;
            capacity_ = SIZE;
            //std::cout << "try to allocate " << (sizeof(T) * capacity_) << " bytes" << std::endl;
            data_ = static_cast<T*>(::operator new(sizeof(T) * capacity_));
        }

        //std::cout << "allocated " << n << " items by address: " << (data_ + (allocatedCount_ - 1)) << std::endl;
        return data_ + (allocatedCount_ - 1);
    }

    void deallocate(T* p, std::size_t n) {
        //std::cout << "deallocate " << n << " items" << std::endl;

        if (data_ && allocatedCount_ > 0) {
            if (++deallocatedCount_ == allocatedCount_) {
                ::operator delete(data_);
                data_ = nullptr;
                //std::cout << "free memory" << std::endl;
            }
        }
    }

    // template<typename UU>
    // struct rebind {
    //     typedef Allocator<UU> other;
    // };

private:
    T* data_ = nullptr;
    std::size_t allocatedCount_ = 0;
    std::size_t deallocatedCount_ = 0;
    std::size_t capacity_ = 0;
};

template<typename T, typename U>
constexpr bool operator== (const Allocator<T>& lhs, const Allocator<U>& rhs) noexcept {
    return lhs.data_ == rhs.data_;
}

template<typename T, typename U>
constexpr bool operator!= (const Allocator<T>& lhs, const Allocator<U>& rhs) noexcept {
    return lhs.data_ != rhs.data_;
}