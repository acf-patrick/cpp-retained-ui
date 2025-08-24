#pragma once

#include "./data.h"

#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>

template <typename T, typename OtherType>
struct IsInVariant : std::false_type {};

template <typename T, typename... Types>
struct IsInVariant<T, std::variant<Types...>> : std::disjunction<std::is_same<T, Types>...> {};

namespace event {

class Event {
  public:
    template <typename T>
    bool isOfType() const {
        return std::holds_alternative<T>(_data);
    }

    template <typename T>
    T unwrapData() const {
        if (auto data = std::get_if<T>(&_data))
            return *data;

        throw std::logic_error("[Event] unwrapping wrong type");
    }

    template <typename T>
    std::optional<T> getIf() const {
        if (auto data = std::get_if<T>(&_data))
            return *data;

        return std::nullopt;
    }

    std::string getName() const;

  private:
    using EventData = std::variant<
        event::data::MouseDown,
        event::data::MouseUp,
        event::data::Click,
        event::data::DoubleClick,
        event::data::MouseMove,
        event::data::MouseLeave,
        event::data::MouseEnter,
        event::data::MouseOut,
        event::data::MouseOver,
        event::data::MouseWheel,
        event::data::KeyDown,
        event::data::KeyUp>;

    static const std::string subtypeNames[];

    template <typename T>
    static constexpr bool isEventSubtype = IsInVariant<T, EventData>::value;

    EventData _data;
};

} // namespace event