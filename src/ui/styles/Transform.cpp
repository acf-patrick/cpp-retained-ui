#include "./Transform.h"

#include "../utils/functions.h"

namespace ui {
namespace style {

bool Transform::isSetToDefault() const {
    if (translation.has_value()) {
        if (auto x = std::get_if<utils::Value<float>>(&translation->x))
            if (x->value != 0.0)
                return false;

        if (auto x = std::get_if<utils::Ratio>(&translation->x))
            if (x->ratio != 1.0)
                return false;

        if (auto y = std::get_if<utils::Value<float>>(&translation->y))
            if (y->value != 0.0)
                return false;

        if (auto y = std::get_if<utils::Ratio>(&translation->y))
            if (y->ratio != 1.0)
                return false;
    }

    if (rotation.has_value()) {
        if (auto deg = std::get_if<utils::AngleDegree>(&rotation->angle))
            if (deg->value != 0)
                return false;

        if (auto rad = std::get_if<utils::AngleRadian>(&rotation->angle))
            if (!utils::isMultipleOf2PI(rad->value))
                return false;
    }

    if (scale.has_value()) {
        if (scale->x != 1.0 || scale->y != 1.0)
            return false;
    }

    return true;
}

} // namespace style
} // namespace ui