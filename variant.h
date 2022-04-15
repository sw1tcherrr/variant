#pragma once

#include "variant_base.h"
#include "variant_comparison.h"
#include "variant_constraints.h"
#include "variant_getters.h"
#include "variant_helpers.h"
#include "variant_visitation.h"

template <class... Ts>
struct variant : private variant_base<(std::is_trivially_destructible_v<Ts> && ...), Ts...> {
private:
  using types = meta::type_sequence<Ts...>;
  using base = variant_base<(std::is_trivially_destructible_v<Ts> && ...), Ts...>;
  using base::destroy;
  using base::discriminator;
  using base::get_storage;
public:
  using base::index;
  using base::valueless_by_exception;
  // default constructor //
  constexpr variant()
      noexcept (std::is_nothrow_default_constructible_v<meta::head_t<types>>)
      requires (std::is_default_constructible_v<meta::head_t<types>>)
      : base(in_place_index<0>) {}

  // inplace constructors //
  template <size_t I, class... Args>
  constexpr explicit variant(in_place_index_t<I>, Args&&... args)
      requires (I < sizeof...(Ts) && std::is_constructible_v<variant_alternative_t<I, variant>, Args...>)
      : base(in_place_index<I>, std::forward<Args>(args)...) {}

  template <class T, class... Args>
  constexpr explicit variant(in_place_type_t<T>, Args&&... args)
      requires (meta::unique_in_pack<T, Ts...> && std::is_constructible_v<T, Args...>)
      : base(in_place_index<meta::idx_by_type_v<T, types>>, std::forward<Args>(args)...) {}

  // converting constructor //
  template <class T>
  requires (
      sizeof...(Ts) > 0
      && !std::is_same_v<variant, std::remove_cvref_t<T>>
      && !converting::is_specialization_size_v<std::remove_cvref_t<T>, in_place_index_t>
      && !converting::is_specialization_types_v<std::remove_cvref_t<T>, in_place_type_t>
      && converting::has_overloads<T, Ts...>
      && std::is_constructible_v<converting::constructible_type_t<T, Ts...>, T>
  )
  constexpr variant(T&& value)
      noexcept (std::is_nothrow_constructible_v<converting::constructible_type_t<T, Ts...>, T>)
      : variant(in_place_type<converting::constructible_type_t<T, Ts...>>, std::forward<T>(value)) {}

  // copy constructors //
  constexpr variant(variant const& other) requires (constraints::default_copy<Ts...>) = default;

  constexpr variant(variant const& other) requires(constraints::copyable<Ts...>)
      : variant(in_place_index<variant_npos>) {
    if (!other.valueless_by_exception()) {
      visitation::indexed_visit([this](auto&& o_value, auto o_idx) {
        discriminator = o_idx;
        std::construct_at(std::addressof(get_storage().template get<o_idx>()), o_value);
      }, other);
    }
  }

  // move constructors //
  constexpr variant(variant&& other)
      noexcept((std::is_nothrow_move_constructible_v<Ts> && ...))
      requires (constraints::default_move<Ts...>) = default;

  constexpr variant(variant&& other)
      noexcept((std::is_nothrow_move_constructible_v<Ts> && ...))
      requires(constraints::moveable<Ts...>)
      : variant(in_place_index<variant_npos>) {
    if (!other.valueless_by_exception()) {
      visitation::indexed_visit([this](auto&& o_value, auto o_idx) {
        discriminator = o_idx;
        std::construct_at(std::addressof(get_storage().template get<o_idx>()), std::move(o_value));
      }, other);
    }
  }

  // copy assignment //
  constexpr variant& operator=(variant const& other) requires (constraints::default_copy_assign<Ts...>) = default;

  constexpr variant& operator=(variant const& other) requires(constraints::copy_assignable<Ts...>) {
    if (this == &other) {
      // nop
    } else if (valueless_by_exception() && other.valueless_by_exception()) {
      // nop
    } else if (!valueless_by_exception() && other.valueless_by_exception()) {
      destroy();
    } else if (index() == other.index()) {
      visitation::indexed_visit([](auto&& v_value, auto&& o_value, auto v_idx, auto o_idx) {
        if constexpr (v_idx == o_idx) {
          v_value = o_value;
        }
      }, *this, other);
    } else {
      visitation::indexed_visit([this, &other](auto&& o_value, auto o_idx) {
        using T = variant_alternative<o_idx, variant>;
        if constexpr (std::is_nothrow_copy_constructible_v<T> || !std::is_nothrow_move_constructible_v<T>) {
          emplace<o_idx>(o_value);
        } else {
          *this = (variant(other));
        }
      }, other);
    }

    return *this;
  }

  // move assignment //
  constexpr variant& operator=(variant&& other)
      noexcept(((std::is_nothrow_move_constructible_v<Ts> && std::is_nothrow_move_assignable_v<Ts>) && ...))
      requires (constraints::default_move_assign<Ts...>) = default;

  constexpr variant& operator=(variant&& other)
      noexcept(((std::is_nothrow_move_constructible_v<Ts> && std::is_nothrow_move_assignable_v<Ts>) && ...))
      requires(constraints::move_assignable<Ts...>) {
    if (this == &other) {
      // nop
    } else if (valueless_by_exception() && other.valueless_by_exception()) {
      // nop
    } else if (!valueless_by_exception() && other.valueless_by_exception()) {
      destroy();
    } else if (index() == other.index()) {
      visitation::indexed_visit([](auto&& v_value, auto&& o_value, auto v_idx, auto o_idx) {
        if constexpr (v_idx == o_idx) {
          v_value = std::move(o_value);
        }
      }, *this, other);
    } else {
      visitation::indexed_visit([this](auto&& o_value, auto o_idx) {
        emplace<o_idx>(std::move(o_value));
      }, other);
    }

    return *this;
  }

  // converting assignment //
  template <class T>
  requires (
      sizeof...(Ts) > 0
      && !std::is_same_v<variant, std::remove_cvref_t<T>>
      && !converting::is_specialization_size_v<std::remove_cvref_t<T>, in_place_index_t>
      && !converting::is_specialization_types_v<std::remove_cvref_t<T>, in_place_type_t>
      && converting::has_overloads<T, Ts...>
      && std::is_constructible_v<converting::constructible_type_t<T, Ts...>, T>
      && std::is_assignable_v<converting::constructible_type_t<T, Ts...>&, T>
  )
  constexpr variant& operator=(T&& t)
      noexcept (std::is_nothrow_constructible_v<converting::constructible_type_t<T, Ts...>, T>
                && std::is_nothrow_assignable_v<converting::constructible_type_t<T, Ts...>&, T>) {
    using T_j = converting::constructible_type_t<T, Ts...>;
    constexpr auto j = meta::idx_by_type_v<T_j, types>;
    if (index() == j) {
      get<j>(*this) = std::forward<T>(t);
    } else if constexpr(std::is_nothrow_constructible_v<T_j, T> || !std::is_nothrow_move_constructible_v<T_j>) {
      emplace<j>(std::forward<T>(t));
    } else {
      emplace<j>(T_j(std::forward<T>(t)));
    }
    return *this;
  }

  // emplace //
  template <class T, class... Args>
  requires (std::is_constructible_v<T, Args...> && meta::unique_in_pack<T, Ts...>)
  constexpr T& emplace(Args&&... args) {
    return emplace<meta::idx_by_type_v<T, types>>(std::forward<Args>(args)...);
  }

  template <size_t I, class... Args>
  requires (std::is_constructible_v<variant_alternative_t<I, variant>, Args...>)
  constexpr variant_alternative_t<I, variant>& emplace(Args&&... args) {
    static_assert(I < sizeof...(Ts));
    destroy();
    discriminator = I;
    try {
      std::construct_at(std::addressof(get_storage().template get<I>()), std::forward<Args>(args)...);
      return get<I>(*this);
    } catch(...) {
      discriminator = variant_npos;
      throw;
    }
  }

  // swap //
  constexpr void swap(variant& other)
  noexcept(((std::is_nothrow_move_constructible_v<Ts> && std::is_nothrow_swappable_v<Ts>) && ...)) {
    if (valueless_by_exception() && other.valueless_by_exception()) {
      // nop
    } else if (index() == other.index()){
      visitation::indexed_visit([](auto&& v_value, auto&& o_value, auto v_idx, auto o_idx) {
        if constexpr (v_idx == o_idx) {
          using std::swap;
          swap(v_value, o_value);
        }
      }, *this, other);
    } else {
      auto tmp = std::move(*this);
      *this = std::move(other);
      other = std::move(tmp);
    }
  }
private:
  // initialize in invalid state, for delegating //
  constexpr explicit variant(in_place_index_t<variant_npos>) : base() {}

  // friends //
  template<size_t I, class Variant>
  friend constexpr decltype(auto) get(Variant&& v);

  template<size_t I, class... Types>
  friend constexpr std::add_pointer_t<const variant_alternative_t<I, variant<Types...>>>
  get_if(const variant<Types...>* pv) noexcept;

  template<size_t I, class... Types>
  friend constexpr std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>
  get_if(variant<Types...>* pv) noexcept;
};


// swap //
template <class... Types>
constexpr void swap(variant<Types...>& lhs, variant<Types...>& rhs ) noexcept(noexcept(lhs.swap(rhs))) {
  lhs.swap(rhs);
}
