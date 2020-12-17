#pragma once

template <class ExtDate>
struct DateAdaptor {
    static ExtDate Date(std::int_fast32_t);
    static ExtDate Date(QuantLib::Day d, QuantLib::Month m, QuantLib::Year y);
    static std::int_fast32_t serialNumber(const ExtDate&);

};

