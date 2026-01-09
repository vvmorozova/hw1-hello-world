#pragma once

#include <string>
#include <iostream>
#include <cstdint>
#include <vector>
#include <list>
#include <string>
#include <type_traits>
#include <tuple>

template <typename T,
             std::enable_if_t<std::is_integral<T>::value, bool> = true>
void print_ip(T t)
{
	std::string delim;
	int bytes = sizeof(T);
	while (bytes > 0) {
		uint8_t byte = (t >> ((bytes - 1) * 8)) % 256;
		delim = bytes != 1 ? "." : "\n";
		std::cout << unsigned(byte) << delim;
		bytes--;
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

// check if container vector of list
template <typename T>
struct is_vec_or_list : std::false_type {};

template <typename T, typename Alloc>
struct is_vec_or_list<std::vector<T, Alloc>> : std::true_type {};

template <typename T, typename Alloc>
struct is_vec_or_list<std::list<T, Alloc>> : std::true_type {};

// vector or list overload
template <typename Container, std::enable_if_t<is_printable<typename Container::value_type>::value && 
		  							is_vec_or_list<Container>::value, bool> = true>
auto print_ip(const Container& cont)
{
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

// for iterating the tuple
template<std::size_t I = 0, typename... Ts>
typename std::enable_if<I == sizeof...(Ts), void>::type
print_tuple(const std::tuple<Ts...>&)
{}

template<std::size_t I = 0, typename... Ts>
typename std::enable_if<I < sizeof...(Ts), void>::type
print_tuple(const std::tuple<Ts...>& t)
{
	const auto &val = std::get<I>(t);
    std::cout << val;
    if (I + 1 < sizeof...(Ts)) {
        std::cout << ".";
	}
    print_tuple<I + 1>(t);
}

// template <typename TupleT, std::size_t TupSize = std::tuple_size_v<TupleT>>
// void printTuple(const TupleT& tp) {
//     printTupleImp(tp, std::make_index_sequence<TupSize>{});
// }

// check if all types in type are same
template <typename... T>
struct all_same_types : std::true_type {};

template <typename F, typename S, typename... Other>
struct all_same_types<F, S, Other...> : std::conditional_t<std::is_same<F, S>::value,
										all_same_types<S, Other...>, std::false_type> {};

template <typename T_TUPLE>
struct is_all_same_tuple;

template <typename... Types>
struct is_all_same_tuple<std::tuple<Types...>> : all_same_types<Types...> {};


// tuple overload
template <typename TUPLE_T>
auto print_ip(TUPLE_T const& t) -> std::enable_if_t<is_all_same_tuple<TUPLE_T>::value, void>
{
	print_tuple(t);
	std::cout << std::endl;
}