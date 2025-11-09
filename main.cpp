#include "lib.h"

#include <map>
#include <iostream>
#include <type_traits>
#include <functional>

size_t factorial(size_t n) {
	if (n <= 1) {
		return 1;
	}

	size_t res = n * factorial(n - 1);
	return res;
}

template <typename T>
struct is_pair : std::false_type {};

template <typename T1, typename T2>
struct is_pair<std::pair<T1, T2>> : std::true_type {};



template <typename Container>
std::enable_if_t<is_pair<typename std::decay<decltype(*std::begin(std::declval<Container>()))>::type>::value, void>
print_container(const Container &container)
{
	int i = 0;
	for (auto elem : container) {
		std::cout << "element №" << i++ << " is " << elem.second << std::endl;
	}
}

template <typename Container>
std::enable_if_t<!is_pair<typename std::decay<decltype(*std::begin(std::declval<Container>()))>::type>::value, void>
print_container(const Container &container)
{
	int i = 0;
	for (auto elem : container) {
		std::cout << "element №" << i++ << " is " << elem << std::endl;
	}
}

int main(int argc, char ** argv) {
	
	if (argc == 2 && std::string(argv[1]) == "-debug") {
		lib_debug = true;
	}
	
	std::cout << "\nmap with default allocator" << std::endl;
	std::map<int, int> defMap;
	for (int i = 0; i < 10; i++) {
		defMap[i] = factorial(i);
	}
	print_container(defMap);

	std::cout << "\nmap with my allocator" << std::endl;
	std::map<int, int, std::less<int>, arena_allocator<std::pair<const int, int>>> myAllocMap;
	for (int i = 0; i < 10; i++) {
		myAllocMap[i] = factorial(i);
	}
	print_container(myAllocMap);

	arena_allocator<std::pair<const int, int>>::cleanup();

	std::cout << "\nmy container with default allocator" << std::endl;
	light_vector<int> vecFact;
	for (int i = 0; i < 10; i++) {
		vecFact.push_back (factorial(i));
	}
	print_container(vecFact);
	
	std::cout << "\nmy container with my allocator" << std::endl;
	light_vector<int, arena_allocator<int>> myAllocVec;
	for (int i = 0; i < 10; i++) {
		myAllocVec.push_back (factorial(i));
	}
	print_container(myAllocVec);


	return 0;
} 


