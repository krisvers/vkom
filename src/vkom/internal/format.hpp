#pragma once

#include <ios>
#include <string>
#include <sstream>
#include <typeinfo>

#include <vkom/object.hpp>
#include <vkom/device.hpp>

namespace vkom {

namespace internal {

namespace fmt {

template<typename T>
inline std::string value(T const& value) {
    return std::to_string(value);
}

template<>
inline std::string value(const char* const& cstr) {
    return std::string(cstr);
}

template<>
inline std::string value(char* const& cstr) {
    return std::string(cstr);
}

template<typename T>
inline std::string value(T* const& ptr) {
    std::stringstream ss = {};
    ss << std::showbase << std::hex << ptr;
    return ss.str();
}

/* C++ type non-sense to support formatting of fixed size cstrings */
template<size_t N>
inline std::string value(char const (&cstr)[N]) {
    return std::string(cstr);
}

template<>
inline std::string value(std::string const& str) {
    return std::string(str);
}

inline std::string_view stream(std::stringstream& ss, std::string_view fmt) {
    ss << fmt;
    return fmt.substr(fmt.size());
}

template<typename T>
inline std::string_view stream(std::stringstream& ss, std::string_view fmt, T const& v) {
    size_t index = fmt.find("{}");
    if (index != fmt.npos) {
        ss << fmt.substr(0, index) << value(v);
        fmt = fmt.substr(index + 2);
        return fmt;
    }

    ss << fmt;
    return fmt.substr(fmt.size());
}

template<typename T, typename... Args>
inline std::string_view stream(std::stringstream& ss, std::string_view fmt, T const& v, Args const&... args) {
    return stream(ss, stream(ss, fmt, v), args...);
}

template<typename... Args>
inline std::string string(std::string_view fmt, Args const&... args) {
    std::stringstream ss = {};
    ss << stream(ss, fmt, args...);
    return ss.str();
}

template<typename... Args>
inline std::string label(IDevice* device, IBase* object, size_t parentLabelMax, std::string_view fmt, Args const&... args) {
    std::string parentLabel = "";
    IChild* child = object->queryInterface<IChild>();
    if (child != nullptr) {
        IParent* parent = child->parent();
        if (parent != nullptr) {
            const char* cstr = device->queryLabel(parent);
            parentLabel = std::string(cstr, std::min(std::strlen(cstr), parentLabelMax));
        }
    }

    std::string user = string(fmt, args...);
    if (!parentLabel.empty()) {
        return string("{} ({}) [Child of \"{}\"]: {}", typeid(object).name(), object, parentLabel, user);
    }

    return string("{} ({}): {}", typeid(object).name(), object, parentLabel, user);
}

template<typename... Args>
inline std::string label(IDevice* device, IBase* object, std::string_view fmt, Args const&... args) {
    return label(device, object, 16, fmt, args...);
}

inline std::string label(IDevice* device, IBase* object, size_t parentLabelMax = 16) {
    std::string parentLabel = "";
    IChild* child = object->queryInterface<IChild>();
    if (child != nullptr) {
        IParent* parent = child->parent();
        if (parent != nullptr) {
            const char* cstr = device->queryLabel(parent);
            parentLabel = std::string(cstr, std::min(std::strlen(cstr), parentLabelMax));
        }
    }

    if (!parentLabel.empty()) {
        return string("{} ({}) [Child of \"{}\"]", typeid(object).name(), object, parentLabel);
    }

    return string("{} ({})", typeid(object).name(), object, parentLabel);
}

}

}

}
