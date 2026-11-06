#include "Function.h"

bool stringToBool(const std::string& str) {
    if (str == "true" || str == "1") {
        return true;
    }
    return false; 
}

std::string GetCurrentDay() {
    auto now = std::chrono::system_clock::now();
    std::string today = std::format("{:%Y%m%d}", std::chrono::floor<std::chrono::days>(now));
    return today;
}
