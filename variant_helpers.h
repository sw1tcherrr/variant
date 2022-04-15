#pragma once

#include <exception>

template <class... Types> struct variant;
template <bool B, class... Types> struct variant_base;

// exceptional //
struct bad_variant_access : std::exception {
  const char* what() const noexcept override {
    return "bad_variant_access";
  }
};

inline constexpr size_t variant_npos = -1;

// inplace //
template <class T>
struct in_place_type_t {
  explicit in_place_type_t() = default;
};

template <class T>
inline constexpr in_place_type_t<T> in_place_type{};

template <size_t I>
struct in_place_index_t {
  explicit in_place_index_t() = default;
};

template <size_t I>
inline constexpr in_place_index_t<I> in_place_index{};

// alternatives //
template <size_t I, class T>
struct variant_alternative {};

template<size_t I, class Head, class... Tail>
struct variant_alternative<I, variant<Head, Tail...>>
    : variant_alternative<I - 1, variant<Tail...>> {};

template<typename Head, typename... Tail>
struct variant_alternative<0, variant<Head, Tail...>> {
  using type = Head;
};

template<size_t I, class Head, class... Tail>
struct variant_alternative<I, const variant<Head, Tail...>>
    : variant_alternative<I - 1, const variant<Tail...>> {};

template<typename Head, typename... Tail>
struct variant_alternative<0, const variant<Head, Tail...>> {
  using type = const Head;
};

template <size_t I, class T>
using variant_alternative_t = typename variant_alternative<I, T>::type;

// size //
template <class T>
struct variant_size {};

template <class... Types>
struct variant_size<variant<Types...>>
    : std::integral_constant<size_t, sizeof...(Types)> {};

template <bool B, class... Types>
struct variant_size<variant_base<B, Types...>>
    : std::integral_constant<size_t, sizeof...(Types)> {};

template <class T>
inline constexpr size_t variant_size_v = variant_size<std::remove_cvref_t<T>>::value;
