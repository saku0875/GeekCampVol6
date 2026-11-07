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

std::string NormalizeText(const std::string& text, int length) {
    if (length <= 0){
        return text;
    }

    int charCount = 0;
    for (int i = 0; i < (int)text.size(); i++){
        if (((unsigned char)text[i] & 0xC0) != 0x80){
            charCount++;
        }
    }

    std::string result = text;
    while (charCount < length){
        result += '　';
        charCount++;
    }
    return result;
}
