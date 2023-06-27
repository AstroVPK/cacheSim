#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "cache.hpp"


int main() {

	std::string cacheSim{std::getenv("CACHE")};
	std::string odyssey_file_path{cacheSim + "/data/odyssey.txt"};
	std::cout << "Opening file " << odyssey_file_path << " to act as main memory." << std::endl;
	std::ifstream odyssey_file(odyssey_file_path);
	std::stringstream odyssey_buffer;
	odyssey_buffer << odyssey_file.rdbuf();
	std::string odyssey{odyssey_buffer.str()};

	Cache<32, 4, 32, 64> P4L1Cache{odyssey};

	std::cout << " cache_size: " << P4L1Cache.get_cache_size()/1024 << " kB" << std::endl;
	std::cout << "   num_ways: " << P4L1Cache.get_num_cache_ways() << std::endl;
	std::cout << "   num_sets: " << P4L1Cache.get_num_cache_sets() << std::endl;
	std::cout << "offset_bits: " << P4L1Cache.get_num_offset_bits() << std::endl;
	std::cout << " index_bits: " << P4L1Cache.get_num_index_bits() << std::endl;
	std::cout << "   tag_bits: " << P4L1Cache.get_num_tag_bits() << std::endl;
	std::cout << "offset_mask: " << P4L1Cache.get_offset_mask() << std::endl;
	std::cout << " index_mask: " << P4L1Cache.get_index_mask() << std::endl;
	std::cout << "   tag_mask: " << P4L1Cache.get_tag_mask() << std::endl;


	std::cout << "Reading from cache" << std::endl;
	for (std::size_t i = 0; i < 8192; ++i) {
		std::cout << std::string{P4L1Cache.read(i)};
	}

	for (std::size_t i = 62; i < 66; ++i) {
		std::cout << std::string{P4L1Cache.read_debug(i)};
	}

	for (std::size_t i = 2046; i < 2050; ++i) {
		std::cout << std::string{P4L1Cache.read_debug(i)};
	}


	for (std::size_t i = 8190; i < 8194; ++i) {
		std::cout << std::string{P4L1Cache.read_debug(i)};
	}

	/*
	Cache<256*1024, 8, 32, 128> P4L2Cache{odyssey};

	std::cout << " cache_size: " << P4L2Cache.get_cache_size()/1024 << " kB" << std::endl;
	std::cout << "   num_ways: " << P4L2Cache.get_num_cache_ways() << std::endl;
	std::cout << "   num_sets: " << P4L2Cache.get_num_cache_sets() << std::endl;
	std::cout << "offset_bits: " << P4L2Cache.get_num_offset_bits() << std::endl;
	std::cout << " index_bits: " << P4L2Cache.get_num_index_bits() << std::endl;
	std::cout << "   tag_bits: " << P4L2Cache.get_num_tag_bits() << std::endl;
	std::cout << "offset_mask: " << P4L2Cache.get_offset_mask() << std::endl;
	std::cout << " index_mask: " << P4L2Cache.get_index_mask() << std::endl;
	std::cout << "   tag_mask: " << P4L2Cache.get_tag_mask() << std::endl;
	*/
}