#pragma once
#define STR(...) (StrBuild() << __VA_ARGS__).str()
#define CSTR(...) (StrBuild() << __VA_ARGS__).str().c_str()
#include <sstream>
#include <string>

class StrBuild {
public:
    template<typename T>
    StrBuild& operator<<(const T& value) {
        ss_ << value;
        return *this;
    }

    std::string str() const {
        return ss_.str();
    }

    operator std::string() const {
        return ss_.str();
    }

private:
    std::stringstream ss_;
};


