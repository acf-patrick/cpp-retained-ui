#pragma once

#include <variant>

namespace ui {
namespace style {

struct IsolationAuto {};
struct IsolationIsolate {};

using Isolation = std::variant<IsolationAuto, IsolationIsolate>;

} // namespace style
}
