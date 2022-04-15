#pragma once

#include "variant_helpers.h"
#include <array>
#include <cstddef>
#include <functional>
#include <utility>

namespace visitation {
// make array //
template<class... Args>
constexpr std::array<typename std::common_type_t<Args...>, sizeof...(Args)> make_array(Args&& ... args) {
  return {std::forward<Args>(args)...};
}

// ftable //
template <bool AddIdx, class F, class... Vs, size_t... Idx>
constexpr auto make_ftable_recursive(std::index_sequence<Idx...>) {
  if constexpr (AddIdx) {
    return [](F&& f, Vs&&... vs) { return std::invoke(std::forward<F>(f), get<Idx>(std::forward<Vs>(vs))...,
        std::integral_constant<size_t, Idx>{}...); };
  } else {
    return [](F&& f, Vs&&... vs) { return std::invoke(std::forward<F>(f), get<Idx>(std::forward<Vs>(vs))...); };
  }
}

template <bool AddIdx, class F, class... Vs, size_t... FixedIdx, size_t... NextIdx, class... VariantsIdxSeqs>
constexpr auto make_ftable_recursive(std::index_sequence<FixedIdx...>, std::index_sequence<NextIdx...>,
                                     VariantsIdxSeqs... is) {
  return make_array(make_ftable_recursive<AddIdx, F, Vs...>(std::index_sequence<FixedIdx..., NextIdx>{}, is...) ...);
}

template <bool AddIdx, class F, class... Vs>
constexpr auto make_ftable() {
  return make_ftable_recursive<AddIdx, F, Vs...>({}, std::make_index_sequence<variant_size_v<Vs>>{}...);
}

// at //
template <class T>
constexpr T&& at_recursive(T&& elt) {
  return std::forward<T>(elt);
}

template <class T, class... Idx>
constexpr decltype(auto) at_recursive(T&& table, size_t i, Idx... idx) {
  return at_recursive(std::forward<T>(table)[i], idx...);
}

template <class T, class... Idx>
constexpr decltype(auto) at(T&& table, Idx... idx) {
  return at_recursive(std::forward<T>(table), idx...);
}

// visit with constexpr indices //
template <class F, class... Vs>
constexpr decltype(auto) indexed_visit(F&& f, Vs&&... vs) {
  constexpr auto ftable = make_ftable<true, F, Vs...>();
  return at(ftable, vs.index()...)(std::forward<F>(f), std::forward<Vs>(vs)...);
}

} // namespace visitation

// visit //
template <class F, class... Vs>
constexpr decltype(auto) visit(F&& f, Vs&&... vs) {
  constexpr auto ftable = visitation::make_ftable<false, F, Vs...>();

  if ((std::forward<Vs>(vs).valueless_by_exception() || ...)) {
    throw bad_variant_access();
  }

  return visitation::at(ftable, vs.index()...)(std::forward<F>(f), std::forward<Vs>(vs)...);
}
