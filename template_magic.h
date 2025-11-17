#pragma once

#include <string>
#include <iostream>

template <typename T,
             std::enable_if_t<std::is_integral<T>::value, bool> = true>
void print_ip(T t)
{
	std::string delim;
	while (t > 0) {
		delim = t / 256 != 0 ? "." : "\n";
		std::cout << t % 256 << delim;
		t /= 256;
	}
}

void print_ip(std::string str);

template <typename T>
class is_printable {
private:
    template <typename U>
    static auto test(int) -> decltype(std::declval<std::ostream&>() << std::declval<U>(),
	std::true_type());

    template <typename>
    static auto test(...) -> std::false_type;

public:
    static constexpr bool value = decltype(test<T>(0))::value;
};

template <typename Container,
          typename = std::enable_if_t<is_printable<typename Container::value_type>::value>>
void print_ip(const Container& cont) {
	int i = 0;
	int len = cont.size();
    for (const auto& elem : cont) {
        std::cout << elem;
		if (i < len - 1) {
			std::cout << ".";
		}
		i++;
    }
    std::cout << std::endl;
}
