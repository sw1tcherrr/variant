#pragma once

#include <utility>

namespace converting {

// is specialization //
template<class T, template<class...> class Target>
struct is_specialization_types : std::false_type {};

template<template<class...> class Target, class... Args>
struct is_specialization_types<Target<Args...>, Target>: std::true_type {};

template<class T, template<class...> class Target>
inline constexpr bool is_specialization_types_v = is_specialization_types<T, Target>::value;

template<class T, template<size_t...> class Target>
struct is_specialization_size : std::false_type {};

template<template<size_t...> class Target, size_t... Args>
struct is_specialization_size<Target<Args...>, Target> : std::true_type {};

template<class T, template<size_t...> class Target>
inline constexpr bool is_specialization_size_v = is_specialization_size<T, Target>::value;

// find T_j //
template <class T_j>
struct array {
  T_j x[1];
};

template <class T_j, class T>
concept consider_F_overload = requires(T&& t) { array<T_j>{ {std::forward<T>(t)} }; };

template <size_t Idx, class T, class T_j>
struct overload {
  using TT = std::remove_cvref_t<T_j>;
  T_j operator()(T_j) requires (std::conditional_t<
                                                  std::is_same_v<T_j, bool>,
                                                  std::is_same<std::remove_cvref_t<T>, bool>,
                                                  std::true_type
                                 >::value
                                 && consider_F_overload<T_j, T>);
  T_j operator()();
};

template <class Idx, class T, class... Ts>
struct all_overloads;

template <size_t... Idx, class T, class... Ts>
struct all_overloads<std::index_sequence<Idx...>, T, Ts...> : overload<Idx, T, Ts>...{
  using overload<Idx, T, Ts>::operator()...;
};

template<class T, class... Ts>
concept has_overloads = requires (all_overloads<std::index_sequence_for<Ts...>, T, Ts...> ao, T&& t) {
    ao.operator()(std::forward<T>(t));
};

template<class T, class... Ts>
struct constructible_type {
  using type = std::invoke_result_t<all_overloads<std::index_sequence_for<Ts...>, T, Ts...>, T>;
};

template <class T, class... Ts>
using constructible_type_t = typename constructible_type<T, Ts...>::type;

} // namespace converting
