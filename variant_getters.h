#pragma once

#include "variant_helpers.h"
#include "variant_meta.h"

template <class... Types> struct variant;

// holds //
template<class T, class... Types>
constexpr bool holds_alternative(const variant<Types...>& v) noexcept {
  return v.index() == meta::idx_by_type_v<T, meta::type_sequence<Types...>>;
}

// get by idx //
template<size_t I, class Variant>
constexpr decltype(auto) get(Variant&& v) {
  if (v.index() != I) {
    throw bad_variant_access();
  }
  return std::forward<Variant>(v).get_storage().template get<I>();
}

// get by type //
template<class T, class Variant>
constexpr decltype(auto) get(Variant&& v) {
  constexpr size_t idx = meta::idx_by_type_v<T, meta::variant_types_t<std::remove_cvref_t<Variant>>>;
  return get<idx>(std::forward<Variant>(v));
}

// get_if by idx //
template<size_t I, class... Types>
constexpr std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>
get_if(variant<Types...>* pv) noexcept {
  if (pv != nullptr && pv->index() == I) {
    return std::addressof(get<I>(*pv));
  }
  return nullptr;
}

template<size_t I, class... Types>
constexpr std::add_pointer_t<const variant_alternative_t<I, variant<Types...>>>
get_if(const variant<Types...>* pv) noexcept {
  if (pv != nullptr && pv->index() == I) {
    return std::addressof(get<I>(*pv));
  }
  return nullptr;
}

// get_if by type //
template<class T, class... Types>
constexpr std::add_pointer_t<T> get_if(variant<Types...>* pv) noexcept {
  constexpr size_t idx = meta::idx_by_type_v<T, meta::type_sequence<Types...>>;
  return get_if<idx>(pv);
}

template<class T, class... Types>
constexpr std::add_pointer_t<const T> get_if(const variant<Types...>* pv) noexcept {
  constexpr size_t idx = meta::idx_by_type_v<T, meta::type_sequence<Types...>>;
  return get_if<idx>(pv);
}
