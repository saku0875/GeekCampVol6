#include "Function.h"

bool stringToBool(const std::string& str) {
    if (str == "true" || str == "1") {
        return true;
    }
    return false; 
}
