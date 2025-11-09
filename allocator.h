#pragma once

#include <cstddef>
#include <memory>
#include <iostream>
#include <cstring>

#include "lib.h"

#define ARENA_SIZE 1024 // 1Kb

template <typename T, size_t reserve_count = ARENA_SIZE>
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

	template <typename U, size_t other_reserve_count>
	arena_allocator(const arena_allocator<U, other_reserve_count> &) {}

	template <typename U>
	arena_allocator(const arena_allocator<U, reserve_count> &) {}

	T* allocate(int n)
	{
		size_t bytes = n * sizeof(T);
		if (!init_arena()) {
			return static_cast<T*>(std::malloc(bytes));
		}

		if (used + bytes > arena_size) {
			size_t new_arena_size = arena_size;
			while (used + bytes > new_arena_size) {
				new_arena_size += reserve_count * sizeof(T);
			}

			char * new_arena = static_cast<char *>(std::malloc(new_arena_size));
			if (!new_arena) {
				std::cerr << "failed to resize arena" << std::endl;
				return static_cast<T*>(std::malloc(bytes));
			}

			std::memcpy(new_arena, arena, used);
			
			current = new_arena + used;
			arena = new_arena;
			arena_size = new_arena_size;
			
			DEBUG_PRINT("succefully expanded arena")
		}

		T* res = reinterpret_cast<T*>(current);
		current += bytes;
		used += bytes;
		
		DEBUG_PRINT("receive memory")
		return res;
	}

	void deallocate(T* ptr,  int n) {
		char * ptr_char = reinterpret_cast<char *>(ptr);
		if (ptr_char < arena || ptr_char + n * sizeof(T) > arena + arena_size) {
			std::cerr << "free pointer outside the arena" << std::endl;
			free(ptr);
		}
		else {
			DEBUG_PRINT("ignore deallocate arena memory")
		}
	}

	static void reset() {
		if (arena) {
			current = arena;
			used = 0;
			DEBUG_PRINT("reset arena")
		}
	} 
	static void cleanup(){
		if (arena) {
			free(arena);
			arena = NULL;
			current = NULL;
			used = 0;
			initialised = false;
			DEBUG_PRINT("freed arena")
		}
	}

	template <typename U>
	struct rebind {
		using other = arena_allocator<U, reserve_count>;
	};

private:
	static bool init_arena() {
		if (initialised) {
			return true;
		}

		arena = static_cast<char *>(std::malloc(reserve_count * sizeof(T)));
		if (arena == NULL) {
			std::cerr << "failed to init arena" << std::endl;
			return false;
		}

		current = arena;
		arena_size = reserve_count * sizeof(T);
		initialised = true;
		used = 0;

		DEBUG_PRINT("init arena")
		return true;
	}

};


template <typename T, size_t reserve_count>
bool arena_allocator<T, reserve_count>::initialised = false;

template <typename T, size_t reserve_count>
char* arena_allocator<T, reserve_count>::arena = NULL;

template <typename T, size_t reserve_count>
char* arena_allocator<T, reserve_count>::current = NULL;

template <typename T, size_t reserve_count>
size_t arena_allocator<T, reserve_count>::used = 0;

template <typename T, size_t reserve_count>
size_t arena_allocator<T, reserve_count>::arena_size = 0;
