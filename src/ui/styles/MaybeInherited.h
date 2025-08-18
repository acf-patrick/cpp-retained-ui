#pragma once

#include <optional>
#include <stdexcept>
#include <variant>

namespace ui {
namespace style {

template <typename T>
class MaybeInherited {
    struct Inherit {
        bool operator<=>(const Inherit &) const = default;
    };

    std::variant<Inherit, T> _value;

  public:
    MaybeInherited() {
        _value = Inherit{};
    }

    static MaybeInherited<T> New(const T &value) {
        MaybeInherited<T> prop;
        prop._value = value;
        return prop;
    }

    static MaybeInherited<T> Inherited() {
        MaybeInherited<T> prop;
        prop._value = Inherit{};
        return prop;
    }

    bool isInherited() const {
        return std::holds_alternative<Inherit>(_value);
    }

    T valueOr(const T &alternative) const {
        if (auto value = std::get_if<T>(&_value))
            return *value;
        return alternative;
    }

    T unwrap() const {
        if (auto value = std::get_if<T>(&_value))
            return *value;
        
        throw std::logic_error("MaybeInherited::unwrap : Unwraping inherited object");
    }

    void setAsInherited() {
        _value = Inherit{};
    }

    void set(const T &value) {
        _value = value;
    }

    MaybeInherited<T> &operator=(const T &value) {
        _value = value;
        return *this;
    }

    bool operator==(const MaybeInherited<T> &rhs) const {
        if ((!isInherited() || !rhs.isInherited()) && (isInherited() || rhs.isInherited()))
            return false;

        if (auto lhsValue = std::get_if<T>(&this->_value)) {
            auto rhsValue = std::get_if<T>(&rhs._value);
            return *lhsValue == *rhsValue;
        }

        return true;
    }

    bool operator!=(const MaybeInherited<T> &rhs) const {
        return !(*this == rhs);
    }
};

} // namespace style
} // namespace ui