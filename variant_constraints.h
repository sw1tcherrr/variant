#pragma once

#include <type_traits>

namespace constraints {

template <class... Ts>
constexpr inline bool default_copy = (std::is_trivially_copy_constructible_v<Ts> && ...);

template <class... Ts>
constexpr inline bool copyable = (std::is_copy_constructible_v<Ts> && ...) && !default_copy<Ts...>;

template <class... Ts>
constexpr inline bool default_move = (std::is_trivially_move_constructible_v<Ts> && ...);

template <class... Ts>
constexpr inline bool moveable = (std::is_move_constructible_v<Ts> && ...) && !default_move<Ts...>;

template <class... Ts>
constexpr inline bool default_copy_assign = (std::is_trivially_copy_constructible_v<Ts> && ...)
                                            && (std::is_trivially_copy_assignable_v<Ts> && ...)
                                            && (std::is_trivially_destructible_v<Ts> && ...);

template <class... Ts>
constexpr inline bool copy_assignable = (std::is_copy_constructible_v<Ts> && ...)
                                        && (std::is_copy_assignable_v<Ts> && ...)
                                        && !default_copy_assign<Ts...>;

template <class... Ts>
constexpr inline bool default_move_assign = (std::is_trivially_move_constructible_v<Ts> && ...)
                                            && (std::is_trivially_move_assignable_v<Ts> && ...)
                                            && (std::is_trivially_destructible_v<Ts> && ...);

template <class... Ts>
constexpr inline bool move_assignable = (std::is_move_constructible_v<Ts> && ...)
                                        && (std::is_move_assignable_v<Ts> && ...)
                                        && !default_move_assign<Ts...>;

} // namespace constraints
