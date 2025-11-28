#include "core.h"

namespace CoreModule {
int calculateSum(int a, int b) {
    return a + b;
}

void CoreHandler::setReady(bool ready) {
    is_ready_ = ready; // 直接赋值，无复杂逻辑
}

bool CoreHandler::isReady() const {
    return is_ready_;
}
}
