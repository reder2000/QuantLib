#pragma once

#include <ql/time/date_adaptor.h>
#include <cstdint>
#include <ql/time/date.hpp>

struct pseudoDate {};

template <>
struct DateAdaptor<pseudoDate> {
    static pseudoDate Date(std::int_fast32_t i) { return pseudoDate(); }
    static pseudoDate Date(QuantLib::Day d, QuantLib::Month m, QuantLib::Year y) {
        return pseudoDate();
    }
    static std::int_fast32_t serialNumber(const pseudoDate& d) { return 0; }
};

template <>
struct fmt::formatter<pseudoDate> : formatter<std::string> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto format(pseudoDate c, FormatContext& ctx) {
        return formatter<std::string>::format("pseudoDate", ctx);
    }
};

struct QuantLibDate {
    QuantLibDate() = default;
    QuantLibDate(const QuantLib::Date& d) : _d(d) {}
    QuantLibDate(QuantLib::Day d, QuantLib::Month m, QuantLib::Year y) : _d(d, m, y) {}
    QuantLib::Date _d;
};

 template <>
 struct fmt::formatter<QuantLibDate>: formatter<std::string> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto format(QuantLibDate c, FormatContext& ctx) {
        std::stringstream ss;
        ss << c._d;
        return formatter<std::string>::format(ss.str(), ctx);
    }
};


template <>
struct DateAdaptor<QuantLibDate> {
    static QuantLibDate Date(std::int_fast32_t i) { return QuantLibDate(QuantLib::Date(i)); }
    static QuantLibDate Date(QuantLib::Day d, QuantLib::Month m, QuantLib::Year y) {
        return QuantLibDate(QuantLib::Date(d,m,y));
    }
    static std::int_fast32_t serialNumber(const QuantLibDate& d) { return d._d.serialNumber(); }
};

//template <>
//struct DateAdaptor<QuantLib::DateLike<QuantLib::Date>> {
//    using T = QuantLib::DateLike<QuantLib::Date>;
//    static T Date(std::int_fast32_t i) {
//        QuantLib::Date res(i);
//        return *static_cast<T*>(&res);
//    }
//    static T Date(QuantLib::Day d, QuantLib::Month m, QuantLib::Year y) {
//        QuantLib::Date res(d, m, y);
//        return *static_cast<T*>(&res);
//    }
//    static std::int_fast32_t serialNumber(const QuantLib::DateLike<QuantLib::Date>& d) { return d.serialNumber(); }
//};
