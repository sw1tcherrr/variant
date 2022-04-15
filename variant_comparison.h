#pragma once

#include "variant_visitation.h"

template <class... Types> struct variant;

template< class... Types >
constexpr bool operator==(const variant<Types...>& v, const variant<Types...>& w) {
  if (v.index() != w.index()) {
    return false;
  }
  if (v.valueless_by_exception()) {
    return true;
  }

  return visitation::indexed_visit([](auto&& v_value, auto&& w_value, auto v_idx, auto w_idx) {
    if constexpr (v_idx == w_idx) return v_value == w_value;
    return false;
  }, v, w);
}

template< class... Types >
constexpr bool operator!=(const variant<Types...>& v, const variant<Types...>& w) {
  if (v.index() != w.index()) {
    return true;
  }
  if (v.valueless_by_exception()) {
    return false;
  }

  return visitation::indexed_visit([](auto&& v_value, auto&& w_value, auto v_idx, auto w_idx) {
    if constexpr (v_idx == w_idx) return v_value != w_value;
    return false;
  }, v, w);
}

template< class... Types >
constexpr bool operator<(const variant<Types...>& v, const variant<Types...>& w) {
  if (w.valueless_by_exception()) {
    return false;
  }
  if (v.valueless_by_exception()) {
    return true;
  }
  if (v.index() < w.index()) {
    return true;
  }
  if (v.index() > w.index()) {
    return false;
  }

  return visitation::indexed_visit([](auto&& v_value, auto&& w_value, auto v_idx, auto w_idx) {
    if constexpr (v_idx == w_idx) return v_value < w_value;
    return false;
  }, v, w);
}

template< class... Types >
constexpr bool operator>(const variant<Types...>& v, const variant<Types...>& w) {
  if (v.valueless_by_exception()) {
    return false;
  }
  if (w.valueless_by_exception()) {
    return true;
  }
  if (v.index() > w.index()) {
    return true;
  }
  if (v.index() < w.index()) {
    return false;
  }

  return visitation::indexed_visit([](auto&& v_value, auto&& w_value, auto v_idx, auto w_idx) {
    if constexpr (v_idx == w_idx) return v_value > w_value;
    return false;
  }, v, w);
}

template< class... Types >
constexpr bool operator<=(const variant<Types...>& v, const variant<Types...>& w) {
  if (v.valueless_by_exception()) {
    return true;
  }
  if (w.valueless_by_exception()) {
    return false;
  }
  if (v.index() < w.index()) {
    return true;
  }
  if (v.index() > w.index()) {
    return false;
  }

  return visitation::indexed_visit([](auto&& v_value, auto&& w_value, auto v_idx, auto w_idx) {
    if constexpr (v_idx == w_idx) return v_value <= w_value;
    return false;
  }, v, w);
}

template< class... Types >
constexpr bool operator>=(const variant<Types...>& v, const variant<Types...>& w) {
  if (w.valueless_by_exception()) {
    return true;
  }
  if (v.valueless_by_exception()) {
    return false;
  }
  if (v.index() > w.index()) {
    return true;
  }
  if (v.index() < w.index()) {
    return false;
  }

  return visitation::indexed_visit([](auto&& v_value, auto&& w_value, auto v_idx, auto w_idx) {
    if constexpr (v_idx == w_idx) return v_value >= w_value;
    return false;
  }, v, w);
}