#pragma once

#include "variant_converting_checks.h"
#include "variant_getters.h"
#include "variant_helpers.h"
#include <cstddef>
#include <type_traits>
#include <utility>

template <bool Trivial, class... Ts>
struct variant_storage {};

template <class Head, class... Tail>
struct variant_storage<true, Head, Tail...> {
  union {
    std::remove_const_t<Head> head;
    variant_storage<true, Tail...> tail;
  };

  constexpr variant_storage() noexcept {
    // user-defined, so not deleted
  }

  constexpr ~variant_storage() noexcept = default;

  template <class... Ts>
  constexpr explicit variant_storage(in_place_index_t<0>, Ts&&... args)
      : head(std::forward<Ts>(args)...) {}

  template <size_t I, class... Ts>
  requires(I > 0)
  constexpr explicit variant_storage(in_place_index_t<I>, Ts&&... args)
      : tail(in_place_index<I - 1>, std::forward<Ts>(args)...) {}

  template <size_t I>
  constexpr auto& get() & noexcept {
    if constexpr (I == 0) {
      return head;
    } else {
      return tail.template get<I - 1>();
    }
  }
  template <size_t I>
  constexpr auto const& get() const& noexcept {
    if constexpr (I == 0) {
      return head;
    } else {
      return tail.template get<I - 1>();
    }
  }
  template <size_t I>
  constexpr auto&& get() && noexcept {
    if constexpr (I == 0) {
      return std::move(head);
    } else {
      return std::move(tail).template get<I - 1>();
    }
  }
  template <size_t I>
  constexpr auto const&& get() const&& noexcept {
    if constexpr (I == 0) {
      return std::move(head);
    } else {
      return std::move(tail).template get<I - 1>();
    }
  }
};

template <class Head, class... Tail>
struct variant_storage<false, Head, Tail...> {
  union {
    std::remove_const_t<Head> head;
    variant_storage<false, Tail...> tail;
  };

  constexpr variant_storage() noexcept {
    // user-defined, so not deleted
  }

  constexpr ~variant_storage() noexcept {
    // user-defined, so not deleted
  }

  template <class... Ts>
  constexpr explicit variant_storage(in_place_index_t<0>, Ts&&... args)
      : head(std::forward<Ts>(args)...) {}

  template <size_t I, class... Ts>
  requires(I > 0)
      constexpr explicit variant_storage(in_place_index_t<I>, Ts&&... args)
      : tail(in_place_index<I - 1>, std::forward<Ts>(args)...) {}

  template <size_t I>
  constexpr auto& get() & noexcept {
    if constexpr (I == 0) {
      return head;
    } else {
      return tail.template get<I - 1>();
    }
  }
  template <size_t I>
  constexpr auto const& get() const& noexcept {
    if constexpr (I == 0) {
      return head;
    } else {
      return tail.template get<I - 1>();
    }
  }
  template <size_t I>
  constexpr auto&& get() && noexcept {
    if constexpr (I == 0) {
      return std::move(head);
    } else {
      return std::move(tail).template get<I - 1>();
    }
  }
  template <size_t I>
  constexpr auto const&& get() const&& noexcept {
    if constexpr (I == 0) {
      return std::move(head);
    } else {
      return std::move(tail).template get<I - 1>();
    }
  }
};
