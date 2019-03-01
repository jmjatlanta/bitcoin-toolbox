#include <functional>

const std::function<std::string(const char*)> G_TRANSLATION_FUN = [](const char* str) { return str; };