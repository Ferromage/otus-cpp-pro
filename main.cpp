#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <tuple>

/// <summary>
/// template function for printing of integral types
/// </summary>                     
template<typename T>
typename std::enable_if<std::is_integral<T>::value, void>::type
print_ip(T t) {
    int n = sizeof(T) * 8;
    auto num = static_cast<uint64_t>(t);
    while (true) {
        std::cout << static_cast<int>((num >> (n - 8)) & 0xFF);
        
        n -= 8;
        if (n == 0) {
            break;
        }

        std::cout << '.';
    }
    std::cout << std::endl;
}

/// <summary>
/// template function for printing of std::string type
/// </summary>                     
template<typename T, typename = typename T::traits_type>
void print_ip(const T& t) {
    std::cout << t << std::endl;
}

/// <summary>
/// template function for printing of std::vector and std::list
/// </summary>                     
template<typename T>
typename std::enable_if<(std::is_same<T, std::vector<typename T::value_type>>::value) || (std::is_same<T, std::list<typename T::value_type>>::value), void>::type
print_ip(const T& t) {
    for (auto it = std::begin(t); it != std::end(t); ++it) {
        std::cout << *it;
        if (std::next(it, 1) != std::end(t)) {
            std::cout << '.';
        }
    }
    std::cout << std::endl;
}


/// <summary>
/// aux functions for std::tuple
/// </summary>                     
template <typename First, typename... T>
struct all_same_type {
    constexpr static bool value = std::is_same<std::tuple<First, T...>, std::tuple<T..., First>>::value;
};

template <typename... T>
struct all_same_type<std::tuple<T...>> : all_same_type<T...> {};

template<std::size_t I = 0, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type
printTuple(std::tuple<Tp...> t, std::ostream& os) {}

template<std::size_t I = 0, typename... Tp>
inline typename std::enable_if<I<sizeof...(Tp), void>::type
printTuple(std::tuple<Tp...> t, std::ostream& os) {
    os << std::get<I>(t) << '.';
    printTuple<I + 1, Tp...>(t, os);
}

/// <summary>
/// template function for printing of std::tuple
/// </summary>                     
template<typename... Args>
typename std::enable_if<all_same_type<Args...>::value, void>::type
print_ip(Args... args) {
    std::ostringstream os;
    printTuple(args..., os);

    if (!os.str().empty()) {
        std::cout << os.str().substr(0, os.str().size() - 1) << std::endl;
    }
}

/// <summary>
/// main function
/// </summary>                     
int main() {
    print_ip( int8_t{-1} ); // 255
    print_ip( int16_t{0} ); // 0.0
    print_ip( int32_t{2130706433} ); // 127.0.0.1
    print_ip( int64_t{8875824491850138409} );// 123.45.67.89.101.112.131.41
    print_ip( std::string{"Hello, World!"} ); // Hello, World!
    print_ip( std::vector<int>{100, 200, 300, 400} ); // 100.200.300.400
    print_ip( std::list<short>{400, 300, 200, 100} ); // 400.300.200.100
    print_ip( std::make_tuple(123, 456, 789, 0) ); // 123.456.789.0
    return 0;
}