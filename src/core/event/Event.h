#pragma once

#include "./data.h"

#include <elements/Element.h>

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <raylib.h>

template <typename T, typename OtherType>
struct IsInVariant : std::false_type {};

template <typename T, typename... Types>
struct IsInVariant<T, std::variant<Types...>> : std::disjunction<std::is_same<T, Types>...> {};

namespace event {

class Event {
  public:
    template <typename EventType>
    Event(EventType &&eventData) {
        _data = eventData;
        _stopPropagationFlag = false;
        _stopImmediatePropagationFlag = false;
    }

    // Helper function for creating new event
    template <typename EventType>
    static std::shared_ptr<Event> New(EventType &&eventData) {
        return std::make_shared<Event>(eventData);
    }

    template <typename T>
    bool isOfType() const {
        return std::holds_alternative<T>(_data);
    }

    template <typename T>
    T unwrapData() const {
        if (auto data = std::get_if<T>(&_data))
            return *data;

        const std::string errorMessage("[Event] unwrapping wrong type");
        TraceLog(LOG_FATAL, errorMessage.c_str());
        throw std::logic_error(errorMessage);
    }

    template <typename T>
    std::optional<T> getIf() const {
        if (auto data = std::get_if<T>(&_data))
            return *data;

        return std::nullopt;
    }

    // Stops bubbling further up the tree
    void stopPropagation();

    // Prevents other handlers on the same element from firing
    void stopImmediatePropagation();

    std::shared_ptr<ui::element::Element> getCurrentTarget() const;
    std::shared_ptr<ui::element::Element> getTarget() const;

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

    template <typename T>
    static constexpr bool isEventSubtype = IsInVariant<T, EventData>::value;

    void setCurrentTarget(std::shared_ptr<ui::element::Element> target);
    void setTarget(std::shared_ptr<ui::element::Element> target);

  private:
    static const std::string subtypeNames[];

    EventData _data;
    bool _stopImmediatePropagationFlag;
    bool _stopPropagationFlag;
    std::weak_ptr<ui::element::Element> _currentTarget;
    std::weak_ptr<ui::element::Element> _target;

    friend class EventManager;
};

} // namespace event