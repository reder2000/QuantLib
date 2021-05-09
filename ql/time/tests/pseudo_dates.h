#pragma once

#include <ql/time/date_adaptor.h>
#include <cstdint>
#include <ql/time/date_like.hpp>
#ifdef USING_BLPAPI
#include <blpapi_datetime.h>
using blpapi_datetime = BloombergLP::blpapi::Datetime;
#endif
#include <iostream> 

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

#ifdef USING_BLPAPI

namespace QuantLib {
    //template <>
    //inline typename DateLike<blpapi_datetime>::serial_type
    //DateLike<blpapi_datetime>::minimumSerialNumber() {
    //    return 25570;
    //}

    template <>
    inline bool operator==(const DateLike<blpapi_datetime>& d1, const DateLike<blpapi_datetime>& d2) {
        return d1.asExtDate() == d2.asExtDate();
    }
    template <>
    inline bool operator<(const DateLike<blpapi_datetime>& d1,
                           const DateLike<blpapi_datetime>& d2) {
        return d1.asExtDate() < d2.asExtDate();
    }
}

template <>
struct DateAdaptor<blpapi_datetime> {
    static blpapi_datetime Date(std::int_fast32_t i);
    static blpapi_datetime Date(QuantLib::Day d, QuantLib::Month m, QuantLib::Year y);
    static std::int_fast32_t serialNumber(const blpapi_datetime& d);
};

inline blpapi_datetime DateAdaptor<blpapi_datetime>::Date(QuantLib::Day d, QuantLib::Month m, QuantLib::Year y) {
    //std::cout << fmt::format("Date {},{},{}\n", d, m, y);
    blpapi_datetime res(y,m,d);
    return res;
}

inline blpapi_datetime DateAdaptor<blpapi_datetime>::Date(std::int_fast32_t i) {
    QuantLib::Date d(i);
//    std::cout << fmt::format("Date {},{},{}\n", d.year(), d.month(), d.dayOfMonth());
    //blpapi_datetime res(d.year(), d.month(), d.dayOfMonth());
    auto t = d.year_month_day();
    blpapi_datetime res(t.year, t.month, t.day);
    return res;
}

inline std::int_fast32_t DateAdaptor<blpapi_datetime>::serialNumber(const blpapi_datetime& d) {
    //std::cout << fmt::format("serialNumber {},{},{}\n", d.day(),
    //                         static_cast<QuantLib::Month>(d.month()), d.year());
    if (d == blpapi_datetime())
        return QuantLib::Date().serialNumber();
    QuantLib::Date qd(d.day(), static_cast<QuantLib::Month>(d.month()), d.year());
    return qd.serialNumber();
}

 template <>
struct fmt::formatter<blpapi_datetime> : formatter<std::string> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto format(blpapi_datetime c, FormatContext& ctx) {
        std::stringstream ss;
        if (c != blpapi_datetime())
            ss << c;
        else
            ss << "null";
        return formatter<std::string>::format(ss.str(), ctx);
    }
};

#endif
