#include <cmath>
#include <vector>
#include <array>
#include <unordered_map>
#include <bitset>
#include <iostream>
#include <climits>
#include <cstdio> 

#include "utils.hpp"


template <std::size_t cache_size_bytes, std::size_t num_cache_ways, std::size_t address_size_bits=64, std::size_t cacheline_length_bytes=64>
class Cache {
	private:
		std::size_t size, num_ways, address_size, cacheline_length, num_sets, offset_bits, index_bits, tag_bits;
		unsigned int offset_mask, index_mask, tag_mask, clk;
		std::array<std::unordered_map<unsigned int, std::tuple<bool, unsigned int, unsigned int, std::array<char, cacheline_length_bytes>>>, num_cache_ways> ways;
		std::array<size_t, num_cache_ways> way_ctr;
		std::string mem;
	public:

		Cache(std::string Memory);

		std::bitset<sizeof(unsigned int)*CHAR_BIT> get_offset_mask();

		std::bitset<sizeof(unsigned int)*CHAR_BIT> get_index_mask();

		std::bitset<sizeof(unsigned int)*CHAR_BIT> get_tag_mask();

		std::size_t get_cache_size();

		std::size_t get_num_cache_ways();

		std::size_t get_num_cache_sets();

		std::size_t get_num_offset_bits();

		std::size_t get_num_index_bits();

		std::size_t get_num_tag_bits();

		char read(std::size_t address);

		char read_debug(std::size_t address);
};

template <std::size_t cache_size_bytes, std::size_t num_cache_ways, std::size_t address_size_bits, std::size_t cacheline_length_bytes>
Cache<cache_size_bytes, num_cache_ways, address_size_bits, cacheline_length_bytes>::Cache(std::string Memory) {
	
	clk = 0;

	size = cache_size_bytes;
	num_ways = num_cache_ways;
	mem = Memory;
	address_size = address_size_bits;
	cacheline_length = cacheline_length_bytes;
	num_sets = static_cast<unsigned int>(size/(num_ways*cacheline_length));
	
	// Set the offset mask
	offset_bits = static_cast<unsigned int>(log2(cacheline_length));
	offset_mask = bitmask<unsigned int>(offset_bits);

	// Set the index mask
	index_bits = static_cast<unsigned int>(log2(num_sets));
	index_mask = bitmask<unsigned int>(index_bits)<<offset_bits;

	// Set the tag mask
	tag_bits = address_size_bits - index_bits - offset_bits;
	tag_mask = bitmask<unsigned int>(tag_bits)<<(index_bits+offset_bits);

	size_t max_index = static_cast<size_t>(std::pow(2.0, static_cast<double>(index_bits))) - 1;
	size_t max_offset = static_cast<size_t>(std::pow(2.0, static_cast<double>(offset_bits))) - 1;

	size_t way_num = 0;

	for (auto & way : ways) {
		way_ctr[way_num] = 0;
		way_num += 1;
		for (unsigned int index = 0; index < max_index; ++index) {
			std::get<bool>(way[index]) = false;
			std::get<1>(way[index]) = 0; // Set last used clk to 0
			std::get<2>(way[index]) = 0; // Set tag to zero
			for (unsigned int offset = 0; offset < max_offset; ++offset) {
				std::get<std::array<char, cacheline_length_bytes>>(way[index])[offset] = 0;
			}
		}
	}
}

template <std::size_t cache_size_bytes, std::size_t num_cache_ways, std::size_t address_size_bits, std::size_t cacheline_length_bytes> 
std::bitset<sizeof(unsigned int)*CHAR_BIT> Cache<cache_size_bytes, num_cache_ways, address_size_bits, cacheline_length_bytes>::get_offset_mask() {
	return std::bitset<sizeof(unsigned int)*CHAR_BIT>(offset_mask);
}

template <std::size_t cache_size_bytes, std::size_t num_cache_ways, std::size_t address_size_bits, std::size_t cacheline_length_bytes>
std::bitset<sizeof(unsigned int)*CHAR_BIT> Cache<cache_size_bytes, num_cache_ways, address_size_bits, cacheline_length_bytes>::get_index_mask() {
	return std::bitset<sizeof(unsigned int)*CHAR_BIT>(index_mask);
}

template <std::size_t cache_size_bytes, std::size_t num_cache_ways, std::size_t address_size_bits, std::size_t cacheline_length_bytes>
std::bitset<sizeof(unsigned int)*CHAR_BIT> Cache<cache_size_bytes, num_cache_ways, address_size_bits, cacheline_length_bytes>::get_tag_mask() {
	return std::bitset<sizeof(unsigned int)*CHAR_BIT>(tag_mask);
}

template <std::size_t cache_size_bytes, std::size_t num_cache_ways, std::size_t address_size_bits, std::size_t cacheline_length_bytes>
std::size_t Cache<cache_size_bytes, num_cache_ways, address_size_bits, cacheline_length_bytes>::get_cache_size() {
	return size;
}

template <std::size_t cache_size_bytes, std::size_t num_cache_ways, std::size_t address_size_bits, std::size_t cacheline_length_bytes>
std::size_t Cache<cache_size_bytes, num_cache_ways, address_size_bits, cacheline_length_bytes>::get_num_cache_ways() {
	return num_ways;
}

template <std::size_t cache_size_bytes, std::size_t num_cache_ways, std::size_t address_size_bits, std::size_t cacheline_length_bytes>
std::size_t Cache<cache_size_bytes, num_cache_ways, address_size_bits, cacheline_length_bytes>::get_num_cache_sets() {
	return num_sets;
}

template <std::size_t cache_size_bytes, std::size_t num_cache_ways, std::size_t address_size_bits, std::size_t cacheline_length_bytes>
std::size_t Cache<cache_size_bytes, num_cache_ways, address_size_bits, cacheline_length_bytes>::get_num_offset_bits() {
	return offset_bits;
}

template <std::size_t cache_size_bytes, std::size_t num_cache_ways, std::size_t address_size_bits, std::size_t cacheline_length_bytes>
std::size_t Cache<cache_size_bytes, num_cache_ways, address_size_bits, cacheline_length_bytes>::get_num_index_bits() {
	return index_bits;
}

template <std::size_t cache_size_bytes, std::size_t num_cache_ways, std::size_t address_size_bits, std::size_t cacheline_length_bytes>
std::size_t Cache<cache_size_bytes, num_cache_ways, address_size_bits, cacheline_length_bytes>::get_num_tag_bits() {
	return tag_bits;
}

template <std::size_t cache_size_bytes, std::size_t num_cache_ways, std::size_t address_size_bits, std::size_t cacheline_length_bytes>
char Cache<cache_size_bytes, num_cache_ways, address_size_bits, cacheline_length_bytes>::read(std::size_t address) {

	unsigned int tag = (address & tag_mask)>>(offset_bits+index_bits);
	unsigned int index = (address & index_mask)>>(offset_bits);
	unsigned int offset = address & offset_mask;

	char data = static_cast<char>(0);
	bool found = false;

	// See if requested datum is already in cache
	for (std::size_t way_num = 0; way_num < num_ways; ++way_num) {
		if (std::get<bool>(ways[way_num][index]) == true) {
			if (std::get<2>(ways[way_num][index]) == tag) {
				std::get<1>(ways[way_num][index]) = clk;
				data = std::get<std::array<char, cacheline_length_bytes>>(ways[way_num][index])[offset];
				found = true;
			}
		}
		way_ctr[way_num] = std::get<1>(ways[way_num][index]);
	}

	// Check to see if data_ptr is not null (data was found in cache) or is null (data was not found)
	if (found == false) {
		// Decide which way to put the data in
		/* We keep track of the last time that each way was used & copy the usage values to way_ctr */
		/* If a way is empty, way_ctr should automatically be zero since it was never used */
		/* So the way with the lowest way_ctr value is automatically the way to use */ 
		auto wayitr = std::min_element(std::begin(way_ctr), std::end(way_ctr));
		size_t lru_way = std::distance(std::begin(way_ctr), wayitr);
		std::get<bool>(ways[lru_way][index]) = true;
		std::get<1>(ways[lru_way][index]) = clk;
		std::get<2>(ways[lru_way][index]) = tag;
		std::size_t wrapped_addr_start = ((address%mem.length())/cacheline_length)*cacheline_length;
		for (size_t off = 0; off < cacheline_length; ++off) {
			std::get<std::array<char, cacheline_length_bytes>>(ways[lru_way][index])[off] = mem[wrapped_addr_start + off];	
		}

		data = std::get<std::array<char, cacheline_length_bytes>>(ways[lru_way][index])[offset];
	}

	clk += 1;

	return data;
}

template <std::size_t cache_size_bytes, std::size_t num_cache_ways, std::size_t address_size_bits, std::size_t cacheline_length_bytes>
char Cache<cache_size_bytes, num_cache_ways, address_size_bits, cacheline_length_bytes>::read_debug(std::size_t address) {

	unsigned int tag = (address & tag_mask)>>(offset_bits+index_bits);
	unsigned int index = (address & index_mask)>>(offset_bits);
	unsigned int offset = address & offset_mask;

	std::cout << std::endl;
	std::cout << "address: " << address << std::endl;
	std::cout << "    tag: " << std::bitset<sizeof(unsigned int)*CHAR_BIT>(tag) << std::endl;
	std::cout << "  index: " << std::bitset<sizeof(unsigned int)*CHAR_BIT>(index) << std::endl;
	std::cout << " offset: " << std::bitset<sizeof(unsigned int)*CHAR_BIT>(offset) << std::endl;

	char data = static_cast<char>(0);
	bool found = false;

	// See if requested datum is already in cache
	for (std::size_t way_num = 0; way_num < num_ways; ++way_num) {
		std::cout << "Checking way: " << way_num << std::endl;
		std::cout << "std::get<bool>(ways[" << way_num << "][" << index << "]): " << std::get<bool>(ways[way_num][index]) << std::endl;
		if (std::get<bool>(ways[way_num][index]) == true) {
			std::cout << "std::get<2>(ways[" << way_num << "][" << index << "]): " << std::get<2>(ways[way_num][index]) << " == tag: " << tag << " is " << (std::get<2>(ways[way_num][index]) == tag) << std::endl;
			if (std::get<2>(ways[way_num][index]) == tag) {
				std::get<1>(ways[way_num][index]) = clk;
				std::cout << "std::get<1>(ways[" << way_num << "][" << index << "]): " << std::get<1>(ways[way_num][index]) << std::endl;
				data = std::get<std::array<char, cacheline_length_bytes>>(ways[way_num][index])[offset];
				found = true;
				std::cout << "found: " << found << "; Found in cache!" << std::endl;
			}
		}
		way_ctr[way_num] = std::get<1>(ways[way_num][index]);
	}
	std::cout << "found: " << found << std::endl;
	printArray("way_ctr", way_ctr);

	// Check to see if data_ptr is not null (data was found in cache) or is null (data was not found)
	if (found == false) {
		std::cout << "found: " << found << "; Cache miss! Going to memory..." << std::endl;
		// Decide which way to put the data in
		/* We keep track of the last time that each way was used & copy the usage values to way_ctr */
		/* If a way is empty, way_ctr should automatically be zero since it was never used */
		/* So the way with the lowest way_ctr value is automatically the way to use */ 
		auto wayitr = std::min_element(std::begin(way_ctr), std::end(way_ctr));
		size_t lru_way = std::distance(std::begin(way_ctr), wayitr);
		std::cout << "lru_way: " << lru_way << std::endl;
		std::get<bool>(ways[lru_way][index]) = true;
		std::get<1>(ways[lru_way][index]) = clk;
		std::get<2>(ways[lru_way][index]) = tag;
		std::cout << "Setting way: " << lru_way << std::endl;
		std::cout << "std::get<bool>(ways[" << lru_way << "][" << index << "]): " << std::get<bool>(ways[lru_way][index]) << std::endl;
		std::cout << "std::get<2>(ways[" << lru_way << "][" << index << "]): " << std::get<2>(ways[lru_way][index]) << std::endl;
		std::cout << "std::get<1>(ways[" << lru_way << "][" << index << "]): " << std::get<1>(ways[lru_way][index]) << std::endl;
		std::cout << "Reading cacheline into cache..." << std::endl;
		std::size_t wrapped_addr_start = ((address%mem.length())/cacheline_length)*cacheline_length;
		std::cout << "wrapped_addr_start: " << wrapped_addr_start << std::endl;
		for (size_t off = 0; off < cacheline_length; ++off) {
			std::get<std::array<char, cacheline_length_bytes>>(ways[lru_way][index])[off] = mem[wrapped_addr_start + off];	
		}

		data = std::get<std::array<char, cacheline_length_bytes>>(ways[lru_way][index])[offset];
	}

	clk += 1;

	return data;
}