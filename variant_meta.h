#pragma once

#include <cstddef>
#include <type_traits>

namespace meta {

template <class... Ts>
struct type_sequence {};

// head //
template <class T> struct head {};

template <class H, class... Ts>
struct head<type_sequence<H, Ts...>> {
  using type = H;
};

template <class T>
using head_t = typename head<T>::type;

// tail //
template <class T> struct tail {};

template <class H, class... Ts>
struct tail<type_sequence<H, Ts...>> {
  using type = type_sequence<Ts...>;
};

template <class T>
using tail_t = typename tail<T>::type;

// cons //
template <class T, class List> struct cons {};

template <class T, class... Ts>
struct cons<T, type_sequence<Ts...>> {
  using type = type_sequence<T, Ts...>;
};

template <class T, class List>
using cons_t = typename cons<T, List>::type;

// find //
constexpr inline size_t npos = -1;

constexpr size_t find_first(const bool* const same, const size_t cnt, size_t idx = 0) {
  for (; idx < cnt; ++idx) {
    if (same[idx]) {
      return idx;
    }
  }
  return npos;
}

template <class T, class List> struct find {};

template <class T, class... Ts>
struct find<T, type_sequence<Ts...>> {
  static constexpr bool same[] = {std::is_same_v<T, Ts>...};
  static constexpr size_t value = find_first(same, sizeof...(Ts), 0);
};

template <class T, class List>
inline constexpr size_t find_v = find<T, List>::value;

// find and check unique //
template <class T, class List> struct find_unique {};

template <class T, class... Ts>
struct find_unique<T, type_sequence<Ts...>> {
  static constexpr bool same[] = {std::is_same_v<T, Ts>...};
  static constexpr size_t first = find_first(same, sizeof...(Ts), 0);
  static constexpr size_t second = (first == npos) ? npos : find_first(same, sizeof...(Ts), first + 1);
  static constexpr size_t value = (second == npos) ? first : npos;
};

template <class T, class List>
inline constexpr size_t find_unique_v = find_unique<T, List>::value;

// find unique with static assert //
template <class T, class List> struct idx_by_type {};

template <class T, class... Ts>
struct idx_by_type<T, type_sequence<Ts...>> {
  static_assert(find_unique_v<T, type_sequence<Ts...>> != npos);
  static constexpr size_t value = find_unique_v<T, type_sequence<Ts...>>;
};

template <class T, class List>
inline constexpr size_t idx_by_type_v = idx_by_type<T, List>::value;

template <class T, class... Ts>
concept unique_in_pack = (find_unique_v<T, type_sequence<Ts...>> != npos);

//// get type_sequence of variant types //

template <class T>
struct variant_types {};

template <template<class...> class Container, class... Types>
struct variant_types<Container<Types...>> {
  using type = type_sequence<Types...>;
};

template <class T>
using variant_types_t = typename variant_types<T>::type;

} // namespace meta
