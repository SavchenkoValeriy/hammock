#pragma once

#include <type_traits>

namespace hammock::utils {
template <class Type, bool Add = true> struct AddConstType;

template <class Type> struct AddConstType<Type, true> {
  using Value = std::add_const_t<Type>;
};

template <class Type> struct AddConstType<Type, false> { using Value = Type; };

template <class Type, bool Add = true>
using AddConst = typename AddConstType<Type, Add>::Value;
} // end namespace hammock::utils
