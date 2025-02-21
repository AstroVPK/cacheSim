#include <cmath>
#include <vector>
#include <array>
#include <unordered_map>
#include <bitset>
#include <iostream>
#include <climits>
#include <cstdio> 

#include "utils.hpp"


template <std::size_t num_cache_sets, std::size_t num_cache_ways, std::size_t address_size_bits=64, std::size_t cacheline_length_bytes=64>
class Cache {
	protected:

		std::size_t size, num_sets, num_ways, address_size, cacheline_length, offset_bits, index_bits, tag_bits;

		unsigned int offset_mask, index_mask, tag_mask, clk;

		std::array<std::array<bool, num_cache_sets>, num_cache_ways> valid;

		std::array<std::array<unsigned int, num_cache_sets>, num_cache_ways> tag_table;

		std::array<std::array<std::array<char, cacheline_length_bytes>, num_cache_sets>, num_cache_ways> data_table;

		std::string mem;

	public:

		Cache() = delete;

		Cache(std::string Memory);

		std::bitset<sizeof(unsigned int)*CHAR_BIT> get_offset_mask() {return std::bitset<sizeof(unsigned int)*CHAR_BIT>(offset_mask);};

		std::bitset<sizeof(unsigned int)*CHAR_BIT> get_index_mask() {return std::bitset<sizeof(unsigned int)*CHAR_BIT>(index_mask);};

		std::bitset<sizeof(unsigned int)*CHAR_BIT> get_tag_mask() {return std::bitset<sizeof(unsigned int)*CHAR_BIT>(tag_mask);};

		std::size_t get_cache_size() {return size;};

		std::size_t get_num_cache_ways() {return num_ways;};

		std::size_t get_num_cache_sets() {return num_sets;};

		std::size_t get_num_offset_bits() {return offset_bits;};

		std::size_t get_num_index_bits() {return index_bits;};

		std::size_t get_num_tag_bits() {return tag_bits;};

		virtual char read(std::size_t address) = 0;

		virtual char read_debug(std::size_t address) = 0;
};


template <std::size_t num_cache_sets, std::size_t num_cache_ways, std::size_t address_size_bits, std::size_t cacheline_length_bytes>
Cache<num_cache_sets, num_cache_ways, address_size_bits, cacheline_length_bytes>::Cache(std::string Memory) {
	
	clk = 0;

	num_sets = num_cache_sets;
	num_ways = num_cache_ways;
	mem = Memory;
	address_size = address_size_bits;
	cacheline_length = cacheline_length_bytes;
	size = num_sets*num_ways*cacheline_length;

	// Set the offset mask
	offset_bits = static_cast<unsigned int>(log2(cacheline_length));
	offset_mask = bitmask<unsigned int>(offset_bits);

	// Set the index mask
	index_bits = static_cast<unsigned int>(log2(num_sets));
	index_mask = bitmask<unsigned int>(index_bits)<<offset_bits;

	// Set the tag mask
	tag_bits = address_size_bits - index_bits - offset_bits;
	tag_mask = bitmask<unsigned int>(tag_bits)<<(index_bits+offset_bits);

	std::size_t max_index = static_cast<std::size_t>(std::pow(2.0, static_cast<double>(index_bits))) - 1;
	std::size_t max_offset = static_cast<std::size_t>(std::pow(2.0, static_cast<double>(offset_bits))) - 1;


	for (std::size_t way_num = 0; way_num < num_ways; ++way_num) {
		for (unsigned int index = 0; index < max_index; ++index) {
			valid[way_num][index] = false;
			tag_table[way_num][index] = 0; // Set tag to zero
			for (unsigned int offset = 0; offset < max_offset; ++offset) {
				data_table[way_num][index][offset] = 0;
			}
		}
	}

}


template <std::size_t num_cache_sets, std::size_t num_cache_ways, std::size_t address_size_bits=64, std::size_t cacheline_length_bytes=64>
class LRUCache : virtual public Cache<num_cache_sets, num_cache_ways, address_size_bits, cacheline_length_bytes> {
	protected:

		std::array<std::array<unsigned int, num_cache_sets>, num_cache_ways> clk_table;

		std::array<std::size_t, num_cache_ways> way_ctr;

	public:

		LRUCache() = delete;

		LRUCache(std::string Memory);

		char read(std::size_t address);

		char read_debug(std::size_t address);

};


template <std::size_t num_cache_sets, std::size_t num_cache_ways, std::size_t address_size_bits, std::size_t cacheline_length_bytes>
LRUCache<num_cache_sets, num_cache_ways, address_size_bits, cacheline_length_bytes>::LRUCache(std::string Memory) :
	Cache<num_cache_sets, num_cache_ways, address_size_bits, cacheline_length_bytes>::Cache(Memory) {

	std::size_t max_index = static_cast<std::size_t>(std::pow(2.0, static_cast<double>(this->index_bits))) - 1;
	
	for (std::size_t way_num = 0; way_num < this->num_ways; ++way_num) {
		way_ctr[way_num] = 0;
		for (unsigned int index = 0; index < max_index; ++index) {
			clk_table[way_num][index] = 0; // Set last used clk to 0
		}
	}

}


template <std::size_t num_cache_sets, std::size_t num_cache_ways, std::size_t address_size_bits, std::size_t cacheline_length_bytes>
char LRUCache<num_cache_sets, num_cache_ways, address_size_bits, cacheline_length_bytes>::read(std::size_t address) {

	unsigned int tag = (address & this->tag_mask)>>(this->offset_bits + this->index_bits);
	unsigned int index = (address & this->index_mask)>>(this->offset_bits);
	unsigned int offset = address & this->offset_mask;

	char data = static_cast<char>(0);
	bool found = false;

	// See if requested datum is already in cache
	for (std::size_t way_num = 0; way_num < this->num_ways; ++way_num) {
		if (this->valid[way_num][index] == true) {
			if (this->tag_table[way_num][index] == tag) {
				clk_table[way_num][index] = this->clk;
				data = this->data_table[way_num][index][offset];
				found = true;
			}
		}
		way_ctr[way_num] = clk_table[way_num][index];
	}

	// Check to see if data_ptr is not null (data was found in cache) or is null (data was not found)
	if (found == false) {
		auto wayitr = std::min_element(std::begin(way_ctr), std::end(way_ctr));
		std::size_t lru_way = std::distance(way_ctr.begin(), wayitr);
		this->valid[lru_way][index] = true;
		clk_table[lru_way][index] = this->clk;
		this->tag_table[lru_way][index] = tag;
		std::size_t wrapped_addr_start = ((address%this->mem.length())/this->cacheline_length)*this->cacheline_length;
		for (std::size_t off = 0; off < this->cacheline_length; ++off) {
			this->data_table[lru_way][index][off] = this->mem[wrapped_addr_start + off];	
		}

		data = this->data_table[lru_way][index][offset];
	}

	this->clk += 1;

	return data;
}


template <std::size_t num_cache_sets, std::size_t num_cache_ways, std::size_t address_size_bits, std::size_t cacheline_length_bytes>
char LRUCache<num_cache_sets, num_cache_ways, address_size_bits, cacheline_length_bytes>::read_debug(std::size_t address) {

	unsigned int tag = (address & this->tag_mask)>>(this->offset_bits + this->index_bits);
	unsigned int index = (address & this->index_mask)>>(this->offset_bits);
	unsigned int offset = address & this->offset_mask;

	std::cout << std::endl;
	std::cout << "address: " << address << std::endl;
	std::cout << "    tag: " << std::bitset<sizeof(unsigned int)*CHAR_BIT>(tag) << std::endl;
	std::cout << "  index: " << std::bitset<sizeof(unsigned int)*CHAR_BIT>(index) << std::endl;
	std::cout << " offset: " << std::bitset<sizeof(unsigned int)*CHAR_BIT>(offset) << std::endl;

	char data = static_cast<char>(0);
	bool found = false;

	// See if requested datum is already in cache
	for (std::size_t way_num = 0; way_num < this->num_ways; ++way_num) {
		std::cout << "Checking way: " << way_num << std::endl;
		std::cout << "valid[" << way_num << "][" << index << "]: " << this->valid[way_num][index] << std::endl;
		if (this->valid[way_num][index] == true) {
			std::cout << "tag_table[" << way_num << "][" << index << "]: " << this->tag_table[way_num][index] << " == tag: " << tag << " is " << (this->tag_table[way_num][index] == tag) << std::endl;
			if (this->tag_table[way_num][index] == tag) {
				clk_table[way_num][index] = this->clk;
				std::cout << "clk_table[" << way_num << "][" << index << "]: " << clk_table[way_num][index] << std::endl;
				data = this->data_table[way_num][index][offset];
				found = true;
				std::cout << "found: " << found << "; Found in cache!" << std::endl;
			}
		}
		way_ctr[way_num] = clk_table[way_num][index];
	}
	std::cout << "found: " << found << std::endl;
	print_container("way_ctr", way_ctr);

	// Check to see if data_ptr is not null (data was found in cache) or is null (data was not found)
	if (found == false) {
		// Decide which way to put the data in
		// We keep track of the last time that each way was used & copy the usage values to way_ctr
		// If a way is empty, way_ctr should automatically be zero since it was never used
		// So the way with the lowest way_ctr value is automatically the way to use
		auto wayitr = std::min_element(std::begin(way_ctr), std::end(way_ctr));
		std::size_t lru_way = std::distance(way_ctr.begin(), wayitr);
		std::cout << "lru_way: " << lru_way << std::endl;
		this->valid[lru_way][index] = true;
		clk_table[lru_way][index] = this->clk;
		this->tag_table[lru_way][index] = tag;
		std::cout << "Setting way: " << lru_way << std::endl;
		std::cout << "valid[" << lru_way << "][" << index << "]: " << this->valid[lru_way][index] << std::endl;
		std::cout << "clk_table[" << lru_way << "][" << index << "]: " << clk_table[lru_way][index] << std::endl;
		std::cout << "tag_table[" << lru_way << "][" << index << "]: " << this->tag_table[lru_way][index] << std::endl;
		std::cout << "Reading cacheline into cache..." << std::endl;
		std::size_t wrapped_addr_start = ((address%this->mem.length())/this->cacheline_length)*this->cacheline_length;
		std::cout << "wrapped_addr_start: " << wrapped_addr_start << std::endl;
		for (std::size_t off = 0; off < this->cacheline_length; ++off) {
			this->data_table[lru_way][index][off] = this->mem[wrapped_addr_start + off];	
		}

		data = this->data_table[lru_way][index][offset];
	}

	this->clk += 1;

	return data;
}


template <std::size_t num_cache_sets, std::size_t num_cache_ways, std::size_t address_size_bits=64, std::size_t cacheline_length_bytes=64>
class LFUCache : virtual public Cache<num_cache_sets, num_cache_ways, address_size_bits, cacheline_length_bytes> {
	protected:

		std::array<std::array<unsigned int, num_cache_sets>, num_cache_ways> freq_table;

		std::array<std::size_t, num_cache_ways> way_ctr;

	public:

		LFUCache() = delete;

		LFUCache(std::string Memory);

		char read(std::size_t address);

		char read_debug(std::size_t address);

};


template <std::size_t num_cache_sets, std::size_t num_cache_ways, std::size_t address_size_bits, std::size_t cacheline_length_bytes>
LFUCache<num_cache_sets, num_cache_ways, address_size_bits, cacheline_length_bytes>::LFUCache(std::string Memory) :
	Cache<num_cache_sets, num_cache_ways, address_size_bits, cacheline_length_bytes>::Cache(Memory) {

	std::size_t max_index = static_cast<std::size_t>(std::pow(2.0, static_cast<double>(this->index_bits))) - 1;
	
	for (std::size_t way_num = 0; way_num < this->num_ways; ++way_num) {
		way_ctr[way_num] = 0;
		for (unsigned int index = 0; index < max_index; ++index) {
			freq_table[way_num][index] = 0; // Set freq to 0
		}
	}

}


template <std::size_t num_cache_sets, std::size_t num_cache_ways, std::size_t address_size_bits, std::size_t cacheline_length_bytes>
char LRUCache<num_cache_sets, num_cache_ways, address_size_bits, cacheline_length_bytes>::read(std::size_t address) {

	unsigned int tag = (address & this->tag_mask)>>(this->offset_bits + this->index_bits);
	unsigned int index = (address & this->index_mask)>>(this->offset_bits);
	unsigned int offset = address & this->offset_mask;

	char data = static_cast<char>(0);
	bool found = false;

	// See if requested datum is already in cache
	for (std::size_t way_num = 0; way_num < this->num_ways; ++way_num) {
		if (this->valid[way_num][index] == true) {
			if (this->tag_table[way_num][index] == tag) {
				freq_table[way_num][index] += 1;
				data = this->data_table[way_num][index][offset];
				found = true;
			}
		}
		way_ctr[way_num] = freq_table[way_num][index];
	}

	// Check to see if data_ptr is not null (data was found in cache) or is null (data was not found)
	if (found == false) {
		auto wayitr = std::min_element(std::begin(way_ctr), std::end(way_ctr));
		std::size_t lru_way = std::distance(way_ctr.begin(), wayitr);
		this->valid[lru_way][index] = true;
		freq_table[lru_way][index] += 1;
		this->tag_table[lru_way][index] = tag;
		std::size_t wrapped_addr_start = ((address%this->mem.length())/this->cacheline_length)*this->cacheline_length;
		for (std::size_t off = 0; off < this->cacheline_length; ++off) {
			this->data_table[lru_way][index][off] = this->mem[wrapped_addr_start + off];	
		}

		data = this->data_table[lru_way][index][offset];
	}

	this->clk += 1;

	return data;
}
