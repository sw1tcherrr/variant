#pragma once

#include "variant_storage.h"
#include "variant_visitation.h"
#include <iostream>

template <bool Trivial, class... Ts>
struct variant_base : private variant_storage<Trivial, Ts...> {
  using storage = variant_storage<Trivial, Ts...>;

  constexpr variant_base() noexcept {}

  size_t discriminator{variant_npos};

  template <size_t I, class... Args>
  constexpr explicit variant_base(in_place_index_t<I>, Args&&... args)
      : storage(in_place_index<I>, std::forward<Args>(args)...), discriminator(I) {}

  constexpr size_t index() const noexcept {
    return discriminator;
  }

  constexpr bool valueless_by_exception() const noexcept {
    return index() == variant_npos;
  }

  // destruct value and invalidate index //
  constexpr void destroy() noexcept {
    if (!valueless_by_exception()) {
      visit([](auto& value) {
        using type = std::remove_cvref_t<decltype(value)>;
        value.~type();
      }, *this);
    }
    discriminator = variant_npos;
  }

  constexpr storage& get_storage() & noexcept {
    return *this;
  }
  constexpr const storage& get_storage() const& noexcept {
    return *this;
  }
  constexpr storage&& get_storage() && noexcept {
    return std::move(*this);
  }
  constexpr const storage&& get_storage() const&& noexcept {
    return std::move(*this);
  }

  // trivial destructor //
  constexpr ~variant_base() noexcept = default;
};

template <class... Ts>
struct variant_base<false, Ts...> : private variant_storage<false, Ts...> {
  using storage = variant_storage<false, Ts...>;

  constexpr variant_base() noexcept {}

  size_t discriminator{variant_npos};

  template <size_t I, class... Args>
  constexpr explicit variant_base(in_place_index_t<I>, Args&&... args)
      : storage(in_place_index<I>, std::forward<Args>(args)...), discriminator(I) {}

  constexpr size_t index() const noexcept {
    return discriminator;
  }

  constexpr bool valueless_by_exception() const noexcept {
    return index() == variant_npos;
  }

  // destruct value and invalidate index //
  constexpr void destroy() noexcept {
    if (!valueless_by_exception()) {
      visit([](auto& value) {
        using type = std::remove_cvref_t<decltype(value)>;
        value.~type();
      }, *this);
    }
    discriminator = variant_npos;
  }

  constexpr storage& get_storage() & noexcept {
    return *this;
  }
  constexpr const storage& get_storage() const& noexcept {
    return *this;
  }
  constexpr storage&& get_storage() && noexcept {
    return std::move(*this);
  }
  constexpr const storage&& get_storage() const&& noexcept {
    return std::move(*this);
  }

  // not trivial destructor //
  constexpr ~variant_base() noexcept {
    destroy();
  }
};
