#pragma once
#include <iostream>
#include <memory>
#define ARENA_SIZE 1024 // 1Kb

template <typename T>
class arena_allocator {
	private:
	static char* arena;
	static char* current;
	
	static size_t arena_size;
	static size_t used;
	
	static bool initialised;
	
public:
	using value_type = T;
	arena_allocator() = default;

	template <typename U>
	arena_allocator(const arena_allocator<U> &) {}

	T* allocate(int n) {
		size_t bytes = n * sizeof(T);
		if (!init_arena()) {
			return static_cast<T*>(std::malloc(bytes));
		}

		if (used + bytes > arena_size) {
			std::cerr << "arena overflow" << std::endl;
			return static_cast<T*>(std::malloc(bytes));
		}

		std::cerr << "receive memory" << std::endl;
		T* res = reinterpret_cast<T*>(current);
		current += bytes;
		used += bytes;

		return res;
	}
	void deallocate(T* ptr,  int n) {
		char * ptr_char = reinterpret_cast<char *>(ptr);
		if (ptr_char < arena || ptr_char + n > arena + arena_size) {
			std::cerr << "free pointer outside the arena" << std::endl;
			free(ptr);
		}
		else {
			std::cout << "ignore deallocate arena memory" << std::endl;
		}
	} 
	static void reset() {
		if (arena) {
			current = arena;
			used = 0;
			std::cout << "reset arena" << std::endl;
		}
	}
	
	static void cleanup() {
		if (arena) {
			free(arena);
			arena = NULL;
			current = NULL;
			used = 0;
			initialised = false;
			std::cout << "freed arena" << std::endl;
		}
	}

private:
	static bool init_arena() {
		if (initialised) {
			return true;
		}

		arena = static_cast<char *>(std::malloc(ARENA_SIZE));
		if (arena == NULL) {
			std::cerr << "failed to init arena" << std::endl;
			return false;
		}

		current = arena;
		arena_size = ARENA_SIZE;
		initialised = true;
		used = 0;

		std::cout << "init arena" << std::endl;
		return true;
	}

};

template <typename T>
bool arena_allocator<T>::initialised = false;

template <typename T>
char* arena_allocator<T>::arena = NULL;

template <typename T>
char* arena_allocator<T>::current = NULL;

template <typename T>
size_t arena_allocator<T>::used = 0;

template <typename T>
size_t arena_allocator<T>::arena_size = 0;