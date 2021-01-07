/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004, 2005, 2006 StatPro Italia srl
 Copyright (C) 2004, 2005, 2006 Ferdinando Ametrano
 Copyright (C) 2006 Katiuscia Manzoni
 Copyright (C) 2006 Toyin Akin
 Copyright (C) 2015 Klaus Spanderen
 Copyright (C) 2020 Leonardo Arcari
 Copyright (C) 2020 Kline s.r.l.

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file date.hpp
    \brief date- and time-related classes, typedefs and enumerations
*/
#pragma once
#ifndef quantlib_date_like_hpp
#define quantlib_date_like_hpp
#include <ql/time/date.hpp> // we hate to, but we somehow still need it until we throw away ql_setting
#include <ql/time/period.hpp>
#include <ql/time/weekday.hpp>
//#include <ql/utilities/null.hpp>
//#include <boost/cstdint.hpp>

#ifdef QL_HIGH_RESOLUTION_DATE
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#endif

#include <utility>
#include <functional>
#include <string>


namespace QuantLib {

    //! Day number
    /*! \ingroup datetime */
    typedef Integer Day;
#if defined(QL_SETTINGS_ARE_REMOVED)
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
#endif //defined(QL_SETTINGS_ARE_REMOVED)
    /*! \relates Month */
    std::ostream& operator<<(std::ostream&, Month);

    //! Year number
    /*! \ingroup datetime */
    typedef Integer Year;

}

#include "date_adaptor.h"

namespace QuantLib {

#ifdef QL_HIGH_RESOLUTION_DATE
    //! Hour number
    /*! \ingroup datetime */
    typedef boost::posix_time::hours::hour_type Hour;

    //! Minute number
    /*! \ingroup datetime */
    typedef boost::posix_time::minutes::min_type Minute;

    //! Second number
    /*! \ingroup datetime */
    typedef boost::posix_time::minutes::sec_type Second;

    //! Millisecond number
    /*! \ingroup datetime */
    typedef boost::posix_time::time_duration::fractional_seconds_type
        Millisecond;

    //! Millisecond number
    /*! \ingroup datetime */
    typedef boost::posix_time::time_duration::fractional_seconds_type
        Microsecond;
#endif

    //! Concrete date class
    /*! This class provides methods to inspect dates as well as methods and
        operators which implement a limited date algebra (increasing and
        decreasing dates, and calculating their difference).

        \ingroup datetime

        \test self-consistency of dates, serial numbers, days of
              month, months, and weekdays is checked over the whole
              date range.
    */

template <class ExtDate>
    class DateLike : public ExtDate {
      public:
          using T = DateLike<ExtDate>;
        //! serial number type
        using serial_type = std::int_fast32_t ;
        //! \name constructors
        //@{
        //! Default constructor returning a null date.
        //Date();
        //! Constructor taking a serial number as given by Applix or Excel.
        //explicit Date(serial_type serialNumber);
        //! More traditional constructor.
        //Date(Day d, Month m, Year y);

#ifdef QL_HIGH_RESOLUTION_DATE
        //! Constructor taking boost posix date time object
        explicit Date(const boost::posix_time::ptime& localTime);
        //! More traditional constructor.
        Date(Day d, Month m, Year y,
             Hour hours, Minute minutes, Second seconds,
             Millisecond millisec = 0, Microsecond microsec = 0);
#endif
        //@}

        //! \name inspectors
        //@{
        Weekday weekday() const;
        Day dayOfMonth() const;
        //! One-based (Jan 1st = 1)
        Day dayOfYear() const;
        Month month() const;
        Year year() const;
        serial_type serialNumber() const;

        auto operator<=>(const DateLike<ExtDate>& d) const {
            return this->serialNumber() <=> d.serialNumber();
        }
        //auto operator<=>(const ExtDate& d) const {
        //    return this->serialNumber() <=>
        //           static_cast<const DateLike<ExtDate>&>(d).serialNumber();
        //}
        const ExtDate & asExtDate() const { return *this; }

#ifdef QL_HIGH_RESOLUTION_DATE
        Hour hours() const;
        Minute minutes() const;
        Second seconds() const;
        Millisecond milliseconds() const;
        Microsecond microseconds() const;

        Time fractionOfDay() const;
        Time fractionOfSecond() const;

        const boost::posix_time::ptime& dateTime() const;
#endif
        //@}

        //! \name date algebra
        //@{
        //! increments date by the given number of days
        DateLike<ExtDate>& operator+=(serial_type days);
        //! increments date by the given period
        DateLike<ExtDate>& operator+=(const Period&);
        //! decrement date by the given number of days
        DateLike<ExtDate>& operator-=(serial_type days);
        //! decrements date by the given period
        DateLike<ExtDate>& operator-=(const Period&);
        //! 1-day pre-increment
        DateLike<ExtDate>& operator++();
        //! 1-day post-increment
        DateLike operator++(int );
        //! 1-day pre-decrement
        DateLike<ExtDate>& operator--();
        //! 1-day post-decrement
        DateLike operator--(int );
        //! returns a new date incremented by the given number of days
        DateLike operator+(serial_type days) const;
        //! returns a new date incremented by the given period
        DateLike operator+(const Period&) const;
        //! returns a new date decremented by the given number of days
        DateLike operator-(serial_type days) const;
        //! returns a new date decremented by the given period
        DateLike operator-(const Period&) const;
        //@}

        //! \name static methods
        //@{
        //! today's date.
        static DateLike todaysDate();
        //! earliest allowed date
        static DateLike minDate();
        //! latest allowed date
        static DateLike maxDate();
        //! whether the given year is a leap one
        static bool isLeap(Year y);
        //! last day of the month to which the given date belongs
        static DateLike endOfMonth(const DateLike<ExtDate>& d);
        //! whether a date is the last day of its month
        static bool isEndOfMonth(const DateLike<ExtDate>& d);
        //! next given weekday following or equal to the given date
        /*! E.g., the Friday following Tuesday, January 15th, 2002
            was January 18th, 2002.

            see http://www.cpearson.com/excel/DateTimeWS.htm
        */
        static DateLike nextWeekday(const DateLike<ExtDate>& d,
                                Weekday w);
        //! n-th given weekday in the given month and year
        /*! E.g., the 4th Thursday of March, 1998 was March 26th,
            1998.

            see http://www.cpearson.com/excel/DateTimeWS.htm
        */
        static DateLike nthWeekday(Size n,
                               Weekday w,
                               Month m,
                               Year y);

#ifdef QL_HIGH_RESOLUTION_DATE
        //! local date time, based on the time zone settings of the computer
        static DateLike localDateTime();
        //! UTC date time
        static DateLike universalDateTime();

        //! underlying resolution of the  posix date time object
        static boost::posix_time::time_duration::tick_type ticksPerSecond();
#endif

        //@}

      private:
        static serial_type minimumSerialNumber();
        static serial_type maximumSerialNumber();
        static void checkSerialNumber(serial_type serialNumber);

#ifdef QL_HIGH_RESOLUTION_DATE
        boost::posix_time::ptime dateTime_;
#else
        //serial_type serialNumber_;
        static DateLike advance(const DateLike<ExtDate>& d, Integer units, TimeUnit);
        static Integer monthLength(Month m, bool leapYear);
        static Integer monthOffset(Month m, bool leapYear);
        static serial_type yearOffset(Year y);
#endif
    };

    /*! \relates Date
        \brief Difference in days between dates
    */
    template <class ExtDate> inline
    typename DateLike<ExtDate>::serial_type operator-(const DateLike<ExtDate>&, const DateLike<ExtDate>&);
    /*! \relates Date
        \brief Difference in days (including fraction of days) between dates
    */
    template <class ExtDate>
    Time daysBetween(const DateLike<ExtDate>&, const DateLike<ExtDate>&);

    /*! \relates Date */
    template <class ExtDate>
    bool operator==(const DateLike<ExtDate>&, const DateLike<ExtDate>&);
    /*! \relates Date */
    template <class ExtDate>
    bool operator!=(const DateLike<ExtDate>&, const DateLike<ExtDate>&);
    /*! \relates Date */
    template <class ExtDate>
    bool operator<(const DateLike<ExtDate>&, const DateLike<ExtDate>&);
    /*! \relates Date */
    template <class ExtDate>
    bool operator<=(const DateLike<ExtDate>&, const DateLike<ExtDate>&);
    /*! \relates Date */
    template <class ExtDate>
    bool operator>(const DateLike<ExtDate>&, const DateLike<ExtDate>&);
    /*! \relates Date */
    template <class ExtDate>
    bool operator>=(const DateLike<ExtDate>&, const DateLike<ExtDate>&);

    /*!
      Compute a hash value of @p d.

      This method makes Date hashable via <tt>boost::hash</tt>.

      Example:

      \code{.cpp}
      #include <boost/unordered_set.hpp>

      boost::unordered_set<ExtDate> set;
      Date d = Date(1, Jan, 2020); 

      set.insert(d);
      assert(set.count(d)); // 'd' was added to 'set'
      \endcode

      \param [in] d Date to hash
      \return A hash value of @p d
      \relates Date
    */
    template <class ExtDate>
    std::size_t hash_value(const DateLike<ExtDate>& d);

    /*! \relates Date */
    template <class ExtDate>
    std::ostream& operator<<(std::ostream&, const DateLike<ExtDate>&);

    namespace ext_detail {

    template <class ExtDate>
        struct short_date_holder {
            explicit short_date_holder(const DateLike<ExtDate> d) : d(d) {}
            DateLike<ExtDate> d;
        };
    template <class ExtDate>
    std::ostream& operator<<(std::ostream&, const short_date_holder<ExtDate>&);

    template <class ExtDate>
    struct long_date_holder {
            explicit long_date_holder(const DateLike<ExtDate>& d) : d(d) {}
            DateLike<ExtDate> d;
        };
    template <class ExtDate>
        std::ostream& operator<<(std::ostream&, const long_date_holder<ExtDate>&);

    template <class ExtDate>
    struct iso_date_holder {
            explicit iso_date_holder(const DateLike<ExtDate>& d) : d(d) {}
            DateLike<ExtDate> d;
        };
    template <class ExtDate>
        std::ostream& operator<<(std::ostream&, const iso_date_holder<ExtDate>&);

    template <class ExtDate>
    struct formatted_date_holder {
            formatted_date_holder(const DateLike<ExtDate>& d, const std::string& f)
            : d(d), f(f) {}
            DateLike<ExtDate> d;
            std::string f;
        };
    template <class ExtDate>
    std::ostream& operator<<(std::ostream&, const formatted_date_holder<ExtDate>&);

#ifdef QL_HIGH_RESOLUTION_DATE
        struct iso_datetime_holder {
            explicit iso_datetime_holder(const DateLike<ExtDate>& d) : d(d) {}
            DateLike d;
        };
        std::ostream& operator<<(std::ostream&, const iso_datetime_holder&);
#endif
    }

    namespace ext_io {

        //! output dates in short format (mm/dd/yyyy)
        /*! \ingroup manips */
        template <class ExtDate>
        ext_detail::short_date_holder<ExtDate> short_date(const DateLike<ExtDate>&);

        //! output dates in long format (Month ddth, yyyy)
        /*! \ingroup manips */
        template <class ExtDate>
        ext_detail::long_date_holder<ExtDate> long_date(const DateLike<ExtDate>&);

        //! output dates in ISO format (yyyy-mm-dd)
        /*! \ingroup manips */
        template <class ExtDate>
        ext_detail::iso_date_holder<ExtDate> iso_date(const DateLike<ExtDate>&);

        //! output dates in user defined format using boost date functionality
        /*! \ingroup manips */
        template <class ExtDate>
        ext_detail::formatted_date_holder<ExtDate> formatted_date(const DateLike<ExtDate>&,
                                                     const std::string& fmt);

#ifdef QL_HIGH_RESOLUTION_DATE
        //! output datetimes in ISO format (YYYY-MM-DDThh:mm:ss,SSSSSS)
        /*! \ingroup manips */
        detail::iso_datetime_holder iso_datetime(const DateLike<ExtDate>&);
#endif

    }

    ////! specialization of Null template for the Date class
    //template <>
    //class Null<ExtDate> {
    //  public:
    //    Null() {}
    //    operator Date() const { return Date(); }
    //};


#ifndef QL_HIGH_RESOLUTION_DATE
    // inline definitions

    template <class ExtDate>
    inline Weekday DateLike<ExtDate>::weekday() const {
        Integer w = serialNumber() % 7;
        return Weekday(w == 0 ? 7 : w);
    }

    template <class ExtDate>
    inline Day DateLike<ExtDate>::dayOfMonth() const {
        return dayOfYear() - monthOffset(month(),isLeap(year()));
    }

    template <class ExtDate>
    inline Day DateLike<ExtDate>::dayOfYear() const {
        return serialNumber() - yearOffset(year());
    }

    template <class ExtDate>
    inline typename DateLike<ExtDate>::serial_type DateLike<ExtDate>::serialNumber() const {
        return DateAdaptor<ExtDate>::serialNumber(static_cast<const ExtDate&>(*this));
    }

    template <class ExtDate>
    inline DateLike<ExtDate> DateLike<ExtDate>::operator+(serial_type days) const {
        auto res = DateAdaptor<ExtDate>::Date(serialNumber() + days);
        return *static_cast<T*>(&res);
    }

    template <class ExtDate>
    inline DateLike<ExtDate> DateLike<ExtDate>::operator-(serial_type days) const {
        return to_DateLike(DateAdaptor<ExtDate>::Date(serialNumber()-days));
    }

    template <class ExtDate>
    inline DateLike<ExtDate> DateLike<ExtDate>::operator+(const Period& p) const {
        return advance(*this,p.length(),p.units());
    }

    template <class ExtDate>
    inline DateLike<ExtDate> DateLike<ExtDate>::operator-(const Period& p) const {
        return advance(*this,-p.length(),p.units());
    }

    template <class ExtDate>
    inline DateLike<ExtDate> DateLike<ExtDate>::endOfMonth(const DateLike<ExtDate>& d) {
        Month m = d.month();
        Year y = d.year();
        auto res = DateAdaptor<ExtDate>::Date(monthLength(m, isLeap(y)), m, y);
        return *static_cast<T*>(&res);
    }

    template <class ExtDate>
    inline bool DateLike<ExtDate>::isEndOfMonth(const DateLike<ExtDate>& d) {
       return (d.dayOfMonth() == monthLength(d.month(), isLeap(d.year())));
    }

    template <class ExtDate>
    inline typename DateLike<ExtDate>::serial_type operator-(const DateLike<ExtDate>& d1, const DateLike<ExtDate>& d2) {
        return d1.serialNumber()-d2.serialNumber();
    }

    template <class ExtDate>
    inline Time daysBetween(const DateLike<ExtDate>& d1, const DateLike<ExtDate>& d2) {
        return Time(d2-d1);
    }

    template <class ExtDate>
    inline bool operator==(const DateLike<ExtDate>& d1, const ExtDate& d2) {
        return (d1.serialNumber() == static_cast<const DateLike<ExtDate>&>(d2).serialNumber());
    }
    template <class ExtDate>
    inline bool operator==(const DateLike<ExtDate>& d1, const DateLike <ExtDate> & d2) {
        return (d1.serialNumber() == d2.serialNumber());
    }
    
    template <class ExtDate>
    inline bool operator!=(const ExtDate& d1, const DateLike<ExtDate> & d2) {
        return d2.serialNumber() != static_cast<const DateLike<ExtDate>&>(d1).serialNumber();
    }
    template <class ExtDate>
    inline bool operator!=(const DateLike<ExtDate>& d2, const ExtDate& d1) {
        return d2.serialNumber() != static_cast<const DateLike<ExtDate>&>(d1).serialNumber();
    }
    template <class ExtDate>
    inline bool operator!=(const DateLike<ExtDate>& d1, const DateLike<ExtDate>& d2) {
        return d1.serialNumber() != d2.serialNumber();
    }
    
    template <class ExtDate>
    inline bool operator<(const DateLike<ExtDate>& d1, const DateLike<ExtDate>& d2) {
        return (d1.serialNumber() < d2.serialNumber());
    }
    
    template <class ExtDate>
    inline bool operator<=(const DateLike<ExtDate>& d1, const ExtDate& d2) {
        return (d1.serialNumber() <= static_cast<const DateLike<ExtDate>&>(d2).serialNumber());
    }
    template <class ExtDate>
    inline bool operator<=(const DateLike<ExtDate>& d1, const DateLike<ExtDate>& d2) {
        return (d1.serialNumber() <= d2.serialNumber());
    }
    template <class ExtDate>
    inline bool operator<(const DateLike<ExtDate>& d1, const ExtDate& d2) {
        return (d1.serialNumber() < static_cast<const DateLike<ExtDate>&>(d2).serialNumber());
    }
    template <class ExtDate>
    inline bool operator<(const ExtDate& d1, const DateLike<ExtDate>& d2) {
        return (static_cast<const DateLike<ExtDate>&>(d1).serialNumber() < d2.serialNumber());
    }
    template <class ExtDate>
    inline bool operator>(const ExtDate& d1 , const DateLike<ExtDate>& d2 ) {
        return (static_cast<const DateLike<ExtDate>&>(d1).serialNumber() > d2.serialNumber());
    }
    template <class ExtDate>
    inline bool operator<=(const ExtDate& d1, const DateLike<ExtDate>& d2) {
        return (static_cast<const DateLike<ExtDate>&>(d1).serialNumber() <= d2.serialNumber());
    }
    template <class ExtDate>
    inline bool operator>(const DateLike<ExtDate>& d1, const DateLike<ExtDate>& d2) {
        return (d1.serialNumber() > d2.serialNumber());
    }
    template <class ExtDate>
    inline bool operator>(const DateLike<ExtDate>& d1, const ExtDate& d2) {
        return (d1.serialNumber() > static_cast<const DateLike<ExtDate>&>(d2).serialNumber());
    }
    template <class ExtDate>
    inline bool operator>=(const DateLike<ExtDate>& d1, const ExtDate& d2) {
        return (d1.serialNumber() >= static_cast<const DateLike<ExtDate>&>(d2).serialNumber());
    }
    template <class ExtDate>
    inline bool operator>=(const DateLike<ExtDate>& d1, const DateLike<ExtDate>& d2) {
        return (d1.serialNumber() >= d2.serialNumber());
    }
#endif
    template <class ExtDate>
    struct Less {
        bool operator()(const ExtDate& d1, const ExtDate& d2) const {
            return to_DateLike<ExtDate>(d1) < to_DateLike<ExtDate>(d2);
        }
    };
}

#include "date_like.cpp"
#endif

template <class ExtDate>
QuantLib::DateLike<ExtDate>& to_DateLike(ExtDate&e) {
    return static_cast<QuantLib::DateLike<ExtDate>&>(e);
}
template <class ExtDate>
const QuantLib::DateLike<ExtDate>& to_DateLike(const ExtDate& e) {
    return static_cast<const QuantLib::DateLike<ExtDate>&>(e);
}
