#pragma once 


template <class T>
struct date_traits {
    using serial_type = int;
    using Time = double;
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

    static bool isLeap(int y);
};

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

}  // namespace QuantLib 
