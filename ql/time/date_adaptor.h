#pragma once

template <class ExtDate>
struct DateAdaptor {
    static ExtDate Date(std::int_fast32_t) {
        static_assert(sizeof(ExtDate)==0,"must be defined");
    }
    static ExtDate Date(QuantLib::Day d, QuantLib::Month m, QuantLib::Year y) {
        static_assert(sizeof(ExtDate) == 0, "must be defined");
    }
    static std::int_fast32_t serialNumber(const ExtDate&) {
        static_assert(sizeof(ExtDate) == 0, "must be defined");
    }

};

