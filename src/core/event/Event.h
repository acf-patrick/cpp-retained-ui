#pragma once

#include <variant>
#include <type_traits>

namespace event {

class Event {
    // template<typename T,
public:

private:
    using EventData = std::variant<>;

    template <typename T, typename Variant>
    struct is_variant_member;


};

}