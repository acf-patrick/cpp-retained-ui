#pragma once

#include <variant>

namespace utils {

template <typename T>
struct Value {
  T value;

  Value(T value) : value(value) {}
};

struct Ratio {
  float ratio;

  Ratio(float ratio) : ratio(ratio) {}
};

struct Auto {};

template <typename ValueType>
using ValueRatioAuto = std::variant<Value<ValueType>, Ratio, Auto>;

template <typename ValueType>
using ValueRatio = std::variant<Value<ValueType>, Ratio>;

}  // namespace utils