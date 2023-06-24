#include <vector>
#include <string>
#include <bitset>
#include <iostream>
#include <climits>


template <typename T>
void printVector(std::string name, std::vector<T> & vec, bool noEndl=false) {
    std::cout << name << ": [";
    for (std::size_t i = 0; i < vec.size() - 1; ++i) {
            std::cout << vec[i] << ", ";
    }
    std::cout << vec[vec.size() - 1] << "]";
    if (noEndl == false) {
        std::cout << std::endl;
    }
}


template <typename T, std::size_t N>
void printArray(std::string name, std::array<T, N> & arr, bool noEndl=false) {
    std::cout << name << ": [";
    for (std::size_t i = 0; i < N - 1; ++i) {
            std::cout << arr[i] << ", ";
    }
    std::cout << arr[N - 1] << "]";
    if (noEndl == false) {
        std::cout << std::endl;
    }
}


template <typename T>
int iterIdx(auto & iter, std::vector<T> arr) {
    return static_cast<int>(std::distance(arr.begin(), iter));
}

template <typename R>
static constexpr R bitmask(unsigned int const onecount) {
    return static_cast<R>(-(onecount != 0)) & (static_cast<R>(-1) >> ((sizeof(R)*CHAR_BIT) - onecount));
}

template <typename R>
static constexpr std::bitset<sizeof(R)*CHAR_BIT> bitmask_bits(unsigned int const onecount) {
    return std::bitset<sizeof(R)*CHAR_BIT>(bitmask<R>(onecount));
}