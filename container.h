#pragma once

#include <memory>
#include <cstddef>

#include "lib.h"

template <class T, class A = std::allocator<T>>
class light_vector {
public:
	using allocator_type = A;
	using value_type = T;
	using reference = T&;
	using const_reference = const T&;
	using size_type = std::size_t;
    using iterator = T*;
    using const_iterator = const T*;

private:
    A alloc_;
    T* data_;
    size_type size_;
    size_type capacity_;

public:
	explicit light_vector(const A& alloc = A())
        : alloc_(alloc), data_(nullptr), size_(0), capacity_(0) {}

    ~light_vector() {
        clear();
        if (data_) {
            std::allocator_traits<A>::deallocate(alloc_, data_, capacity_);
		}
    }

    void push_back(const T& value) {
        if (size_ == capacity_)
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);

        std::allocator_traits<A>::construct(alloc_, data_ + size_, value);
        ++size_;
    }

    void push_back(T&& value) {
        if (size_ == capacity_)
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);

        std::allocator_traits<A>::construct(alloc_, data_ + size_, std::move(value));
        ++size_;
    }

    reference operator[](size_type i) {
        return data_[i];
    }

    const_reference operator[](size_type i) const {
        return data_[i];
    }

    size_type size() const noexcept { return size_; }

    T* data() noexcept { return data_; }
    const T* data() const noexcept { return data_; }

	iterator begin() noexcept { return data_; }
    const_iterator begin() const noexcept { return data_; }
    const_iterator cbegin() const noexcept { return data_; }

    iterator end() noexcept { return data_ + size_; }
    const_iterator end() const noexcept { return data_ + size_; }
    const_iterator cend() const noexcept { return data_ + size_; }

private:
    void reserve(size_type new_cap) {
        if (new_cap <= capacity_) {
			return;
		}

		T* new_data = std::allocator_traits<A>::allocate(alloc_, new_cap);

		for (size_type i = 0; i < size_; i++) {
			std::allocator_traits<A>::construct(alloc_, new_data + i, std::move_if_noexcept(data_[i]));
			std::allocator_traits<A>::destroy(alloc_, data_ + i);
		}

        data_ = new_data;
        capacity_ = new_cap;
    }

    void clear() {
        for (size_type i = 0; i < size_; ++i)
            std::allocator_traits<A>::destroy(alloc_, data_ + i);
        size_ = 0;
    }
};