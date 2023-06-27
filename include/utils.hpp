#include <vector>
#include <string>
#include <bitset>
#include <iostream>
#include <climits>


template <typename C>
void print_container(std::string const & name, C const & container, bool noEndl=false) {
    std::cout << name << ": [";
    for (auto iter = container.begin(); iter < container.end() - 1; ++iter) {
            std::cout << *iter << ", ";
    }
    std::cout << *(container.end() - 1) << "]";
    if (noEndl == false) {
        std::cout << std::endl;
    }
}

template <typename C>
std::size_t iterator_index(auto & iter, C & container) {
    return static_cast<std::size_t>(std::distance(container.begin(), iter));
}

template <typename R>
static constexpr R bitmask(unsigned int const onecount) {
    return static_cast<R>(-(onecount != 0)) & (static_cast<R>(-1) >> ((sizeof(R)*CHAR_BIT) - onecount));
}

template <typename R>
static constexpr std::bitset<sizeof(R)*CHAR_BIT> bitmask_bits(unsigned int const onecount) {
    return std::bitset<sizeof(R)*CHAR_BIT>(bitmask<R>(onecount));
}