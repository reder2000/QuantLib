#pragma once 

#include "period.hpp"
#include "weekday.hpp"

namespace QuantLib {

    //! Month names
    /*! \ingroup datetime */
    enum Month {
        January = 1,
        February = 2,
        March = 3,
        April = 4,
        May = 5,
        June = 6,
        July = 7,
        August = 8,
        September = 9,
        October = 10,
        November = 11,
        December = 12,
        Jan = 1,
        Feb = 2,
        Mar = 3,
        Apr = 4,
        Jun = 6,
        Jul = 7,
        Aug = 8,
        Sep = 9,
        Oct = 10,
        Nov = 11,
        Dec = 12
    };

} // namespace QuantLib

template <class T>
struct date_traits {
    using serial_type = int;
    using Time = double;
    using Day = int;
    /*! \relates Date
    \brief Difference in days (including fraction of days) between dates
*/
    static Time daysBetween(const T&, const T&);
    /*! \relates Date
        \brief Difference in days between dates
    */
    static serial_type onlyDaysBetween(const T&, const T&);
    //! whether a date is the last day of its month
    static bool isEndOfMonth(const T&);

    static QuantLib::Weekday weekday(const T&);

    static Day dayOfMonth(const T&);

    static Day dayOfYear(const T&);

    static QuantLib::Month month(const T&);

    static T plusPeriod(const T&, const QuantLib::Period&);

    static T& inc(T&);

    static T& dec(T&);

    static T add(const T&, serial_type);

    static T& addSelf(T&, serial_type);

    static T& addSelf(T&, const QuantLib::Period&);

    static T endOfMonth(const T&);

};

    inline
    bool isLeap(int y) {
    static const bool YearIsLeap[] = {
        // 1900 is leap in agreement with Excel's bug
        // 1900 is out of valid date range anyway
        // 1900-1909
        true, false, false, false, true, false, false, false, true, false,
        // 1910-1919
        false, false, true, false, false, false, true, false, false, false,
        // 1920-1929
        true, false, false, false, true, false, false, false, true, false,
        // 1930-1939
        false, false, true, false, false, false, true, false, false, false,
        // 1940-1949
        true, false, false, false, true, false, false, false, true, false,
        // 1950-1959
        false, false, true, false, false, false, true, false, false, false,
        // 1960-1969
        true, false, false, false, true, false, false, false, true, false,
        // 1970-1979
        false, false, true, false, false, false, true, false, false, false,
        // 1980-1989
        true, false, false, false, true, false, false, false, true, false,
        // 1990-1999
        false, false, true, false, false, false, true, false, false, false,
        // 2000-2009
        true, false, false, false, true, false, false, false, true, false,
        // 2010-2019
        false, false, true, false, false, false, true, false, false, false,
        // 2020-2029
        true, false, false, false, true, false, false, false, true, false,
        // 2030-2039
        false, false, true, false, false, false, true, false, false, false,
        // 2040-2049
        true, false, false, false, true, false, false, false, true, false,
        // 2050-2059
        false, false, true, false, false, false, true, false, false, false,
        // 2060-2069
        true, false, false, false, true, false, false, false, true, false,
        // 2070-2079
        false, false, true, false, false, false, true, false, false, false,
        // 2080-2089
        true, false, false, false, true, false, false, false, true, false,
        // 2090-2099
        false, false, true, false, false, false, true, false, false, false,
        // 2100-2109
        false, false, false, false, true, false, false, false, true, false,
        // 2110-2119
        false, false, true, false, false, false, true, false, false, false,
        // 2120-2129
        true, false, false, false, true, false, false, false, true, false,
        // 2130-2139
        false, false, true, false, false, false, true, false, false, false,
        // 2140-2149
        true, false, false, false, true, false, false, false, true, false,
        // 2150-2159
        false, false, true, false, false, false, true, false, false, false,
        // 2160-2169
        true, false, false, false, true, false, false, false, true, false,
        // 2170-2179
        false, false, true, false, false, false, true, false, false, false,
        // 2180-2189
        true, false, false, false, true, false, false, false, true, false,
        // 2190-2199
        false, false, true, false, false, false, true, false, false, false,
        // 2200
        false};
    QL_REQUIRE(y >= 1900 && y <= 2200, "year outside valid range");
    return YearIsLeap[y - 1900];
}
