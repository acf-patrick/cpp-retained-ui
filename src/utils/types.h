#pragma once

#include <variant>

namespace utils {

template<typename T>
struct Edges {
  T top;
  T bottom;
  T left;
  T right;
};

template <typename T>
struct Value {
  T value;

  Value(T value) : value(value) {}

  bool operator<=>(const Value<T>&) const = default;
};

struct Ratio {
  float ratio;

  Ratio(float ratio) : ratio(ratio) {}

  bool operator<=>(const Ratio&) const = default;
};

struct Auto {
  bool operator<=>(const Auto&) const = default;
};

template <typename ValueType>
using ValueRatioAuto = std::variant<Value<ValueType>, Ratio, Auto>;

template <typename ValueType>
using ValueRatio = std::variant<Value<ValueType>, Ratio>;

}  // namespace utils