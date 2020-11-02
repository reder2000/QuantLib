/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004, 2005, 2006, 2007 StatPro Italia srl
 Copyright (C) 2006 Piter Dias
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

/*! \file calendar.hpp
    \brief %calendar class
*/

#ifndef quantlib_calendar_hpp
#define quantlib_calendar_hpp

#include <ql/errors.hpp>
#include <ql/time/businessdayconvention.hpp>
#include <ql/shared_ptr.hpp>
#include <set>
#include <vector>
#include <string>


namespace QuantLib {

    //! Units used to describe time periods
    /*! \ingroup datetime */
    enum TimeUnit {
        Days,
        Weeks,
        Months,
        Years,
        Hours,
        Minutes,
        Seconds,
        Milliseconds,
        Microseconds
    };
}

    namespace QuantLib {

    /*! Day's serial number MOD 7;
        WEEKDAY Excel function is the same except for Sunday = 7.

        \ingroup datetime
    */
    enum Weekday {
        Sunday = 1,
        Monday = 2,
        Tuesday = 3,
        Wednesday = 4,
        Thursday = 5,
        Friday = 6,
        Saturday = 7,
        Sun = 1,
        Mon = 2,
        Tue = 3,
        Wed = 4,
        Thu = 5,
        Fri = 6,
        Sat = 7
    };

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

}

    using Day=int;
    using Year=int;

    template <class T>
    struct type_traits {
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

        static QuantLib::Weekday weekday(const T&);

        static Day dayOfMonth(const T&);

        static Day dayOfYear(const T&);

        static QuantLib::Month month(const T&) ;
        
    };

namespace QuantLib {

    class Period;

    //! %calendar class
    /*! This class provides methods for determining whether a date is a
        business day or a holiday for a given market, and for
        incrementing/decrementing a date of a given number of business days.

        The Bridge pattern is used to provide the base behavior of the
        calendar, namely, to determine whether a date is a business day.

        A calendar should be defined for specific exchange holiday schedule
        or for general country holiday schedule. Legacy city holiday schedule
        calendars will be moved to the exchange/country convention.

        \ingroup datetime

        \test the methods for adding and removing holidays are tested
              by inspecting the calendar before and after their
              invocation.
    */
    template <class Date>
    class Calendar {
      protected:
        //! abstract base class for calendar implementations
        class Impl {
          public:
            virtual ~Impl() {}
            virtual std::string name() const = 0;
            virtual bool isBusinessDay(const Date&) const = 0;
            virtual bool isWeekend(Weekday) const = 0;
            std::set<Date> addedHolidays, removedHolidays;
        };
        ext::shared_ptr<Impl> impl_;
      public:
        /*! The default constructor returns a calendar with a null
            implementation, which is therefore unusable except as a
            placeholder.
        */
        Calendar<Date>() {}
        //! \name Calendar<Date> interface
        //@{
        //!  Returns whether or not the calendar is initialized
        bool empty() const;
        //! Returns the name of the calendar.
        /*! \warning This method is used for output and comparison between
                calendars. It is <b>not</b> meant to be used for writing
                switch-on-type code.
        */
        std::string name() const;
        /*! Returns <tt>true</tt> iff the date is a business day for the
            given market.
        */

        /*! Returns the set of added holidays for the given calendar */
        const std::set<Date>& addedHolidays() const;
        
        /*! Returns the set of removed holidays for the given calendar */
        const std::set<Date>& removedHolidays() const;

        bool isBusinessDay(const Date& d) const;
        /*! Returns <tt>true</tt> iff the date is a holiday for the given
            market.
        */
        bool isHoliday(const Date& d) const;
        /*! Returns <tt>true</tt> iff the weekday is part of the
            weekend for the given market.
        */
        bool isWeekend(Weekday w) const;
        /*! Returns <tt>true</tt> iff in the given market, the date is on
            or after the last business day for that month.
        */
        bool isEndOfMonth(const Date& d) const;
        //! last business day of the month to which the given date belongs
        Date endOfMonth(const Date& d) const;

        /*! Adds a date to the set of holidays for the given calendar. */
        void addHoliday(const Date&);
        /*! Removes a date from the set of holidays for the given calendar. */
        void removeHoliday(const Date&);

        /*! Returns the holidays between two dates.

            \deprecated Use the non-static overload.
                        Deprecated in version 1.18.
        */
        ////QL_DEPRECATED
        //static std::vector<Date> holidayList(const Calendar<Date>& calendar,
        //                                     const Date& from,
        //                                     const Date& to,
        //                                     bool includeWeekEnds = false);

        /*! Returns the holidays between two dates. */
        std::vector<Date> holidayList(const Date& from,
                                      const Date& to,
                                      bool includeWeekEnds = false) const;
        /*! Returns the business days between two dates. */
        std::vector<Date> businessDayList(const Date& from,
                                          const Date& to) const;

        /*! Adjusts a non-business day to the appropriate near business day
            with respect to the given convention.
        */
        Date adjust(const Date&,
                    BusinessDayConvention convention = Following) const;
        /*! Advances the given date of the given number of business days and
            returns the result.
            \note The input date is not modified.
        */
        Date advance(const Date&,
                     int n,
                     TimeUnit unit,
                     BusinessDayConvention convention = Following,
                     bool endOfMonth = false) const;
        /*! Advances the given date as specified by the given period and
            returns the result.
            \note The input date is not modified.
        */
        Date advance(const Date& date,
                     const Period& period,
                     BusinessDayConvention convention = Following,
                     bool endOfMonth = false) const;
        /*! Calculates the number of business days between two given
            dates and returns the result.
        */
        typename type_traits<Date>::serial_type businessDaysBetween(const Date& from,
                                              const Date& to,
                                              bool includeFirst = true,
                                              bool includeLast = false) const;
        //@}

      protected:
        //! partial calendar implementation
        /*! This class provides the means of determining the Easter
            Monday for a given year, as well as specifying Saturdays
            and Sundays as weekend days.
        */
        class WesternImpl : public Impl {
          public:
            bool isWeekend(Weekday) const;
            //! expressed relative to first day of year
            static Day easterMonday(Year);
        };
        //! partial calendar implementation
        /*! This class provides the means of determining the Orthodox
            Easter Monday for a given year, as well as specifying
            Saturdays and Sundays as weekend days.
        */
        class OrthodoxImpl : public Impl {
          public:
            bool isWeekend(Weekday) const;
            //! expressed relative to first day of year
            static Day easterMonday(Year);
        };
    };

    /*! Returns <tt>true</tt> iff the two calendars belong to the same
        derived class.
        \relates Calendar<Date>
    */
    template <class Date>
    bool operator==(const Calendar<Date>&, const Calendar<Date>&);

    /*! \relates Calendar<Date> */
    template <class Date>
    bool operator!=(const Calendar<Date>&, const Calendar<Date>&);

    /*! \relates Calendar<Date> */
    template <class Date>
    std::ostream& operator<<(std::ostream&, const Calendar<Date>&);


    // inline definitions

    template <class Date>
    inline bool Calendar<Date>::empty() const {
        return !impl_;
    }

    template <class Date>
    inline std::string Calendar<Date>::name() const {
        QL_REQUIRE(impl_, "no Calendar<Date> implementation provided");
        return impl_->name();
    }

    template <class Date>
    inline const std::set<Date>& Calendar<Date>::addedHolidays() const {
        QL_REQUIRE(impl_, "no Calendar<Date> implementation provided");

        return impl_->addedHolidays;
    }

    template <class Date>
    inline const std::set<Date>& Calendar<Date>::removedHolidays() const {
        QL_REQUIRE(impl_, "no Calendar<Date> implementation provided");

        return impl_->removedHolidays;
    }

    template <class Date>
    inline bool Calendar<Date>::isBusinessDay(const Date& d) const {
        QL_REQUIRE(impl_, "no Calendar<Date> implementation provided");

#ifdef QL_HIGH_RESOLUTION_DATE
        const Date _d(d.dayOfMonth(), d.month(), d.year());
#else
        const Date& _d = d;
#endif

        if (!impl_->addedHolidays.empty() &&
            impl_->addedHolidays.find(_d) != impl_->addedHolidays.end())
            return false;

        if (!impl_->removedHolidays.empty() &&
            impl_->removedHolidays.find(_d) != impl_->removedHolidays.end())
            return true;

        return impl_->isBusinessDay(_d);
    }

    template <class Date>
    inline bool Calendar<Date>::isEndOfMonth(const Date& d) const {
        return (d.month() != adjust(d+1).month());
    }

    template <class Date>
    inline Date Calendar<Date>::endOfMonth(const Date& d) const {
        return adjust(Date::endOfMonth(d), Preceding);
    }

    template <class Date>
    inline bool Calendar<Date>::isHoliday(const Date& d) const {
        return !isBusinessDay(d);
    }

    template <class Date>
    inline bool Calendar<Date>::isWeekend(Weekday w) const {
        QL_REQUIRE(impl_, "no Calendar<Date> implementation provided");
        return impl_->isWeekend(w);
    }

    template <class Date>
    inline bool operator==(const Calendar<Date>& c1, const Calendar<Date>& c2) {
        return (c1.empty() && c2.empty())
            || (!c1.empty() && !c2.empty() && c1.name() == c2.name());
    }

    template <class Date>
    inline bool operator!=(const Calendar<Date>& c1, const Calendar<Date>& c2) {
        return !(c1 == c2);
    }

    template <class Date>
    inline std::ostream& operator<<(std::ostream& out, const Calendar<Date>& c) {
        return out << c.name();
    }

      template <class Date>
    inline void Calendar<Date>::addHoliday(const Date& d) {
        QL_REQUIRE(impl_, "no calendar implementation provided");

#ifdef QL_HIGH_RESOLUTION_DATE
        const Date _d(d.dayOfMonth(), d.month(), d.year());
#else
        const Date& _d = d;
#endif

        // if d was a genuine holiday previously removed, revert the change
        impl_->removedHolidays.erase(_d);
        // if it's already a holiday, leave the calendar alone.
        // Otherwise, add it.
        if (impl_->isBusinessDay(_d))
            impl_->addedHolidays.insert(_d);
    }

     template <class Date>
    inline void Calendar<Date>::removeHoliday(const Date& d) {
        QL_REQUIRE(impl_, "no calendar implementation provided");

#ifdef QL_HIGH_RESOLUTION_DATE
        const Date _d(d.dayOfMonth(), d.month(), d.year());
#else
        const Date& _d = d;
#endif

        // if d was an artificially-added holiday, revert the change
        impl_->addedHolidays.erase(_d);
        // if it's already a business day, leave the calendar alone.
        // Otherwise, add it.
        if (!impl_->isBusinessDay(_d))
            impl_->removedHolidays.insert(_d);
    }

      template <class Date>
    inline Date Calendar<Date>::adjust(const Date& d, BusinessDayConvention c) const {
        QL_REQUIRE(d != Date(), "null date");

        if (c == Unadjusted)
            return d;

        Date d1 = d;
        if (c == Following || c == ModifiedFollowing || c == HalfMonthModifiedFollowing) {
            while (isHoliday(d1))
                ++d1;
            if (c == ModifiedFollowing || c == HalfMonthModifiedFollowing) {
                if (d1.month() != d.month()) {
                    return adjust(d, Preceding);
                }
                if (c == HalfMonthModifiedFollowing) {
                    if (d.dayOfMonth() <= 15 && d1.dayOfMonth() > 15) {
                        return adjust(d, Preceding);
                    }
                }
            }
        } else if (c == Preceding || c == ModifiedPreceding) {
            while (isHoliday(d1))
                --d1;
            if (c == ModifiedPreceding && d1.month() != d.month()) {
                return adjust(d, Following);
            }
        } else if (c == Nearest) {
            Date d2 = d;
            while (isHoliday(d1) && isHoliday(d2)) {
                ++d1;
                --d2;
            }
            if (isHoliday(d1))
                return d2;
            else
                return d1;
        } else {
            QL_FAIL("unknown business-day convention");
        }
        return d1;
    }

      template <class Date>
    inline Date Calendar<Date>::advance(
        const Date& d, int n, TimeUnit unit, BusinessDayConvention c, bool endOfMonth) const {
        QL_REQUIRE(d != Date(), "null date");
        if (n == 0) {
            return adjust(d, c);
        } else if (unit == Days) {
            Date d1 = d;
            if (n > 0) {
                while (n > 0) {
                    ++d1;
                    while (isHoliday(d1))
                        ++d1;
                    --n;
                }
            } else {
                while (n < 0) {
                    --d1;
                    while (isHoliday(d1))
                        --d1;
                    ++n;
                }
            }
            return d1;
        } else if (unit == Weeks) {
            Date d1 = d + n * unit;
            return adjust(d1, c);
        } else {
            Date d1 = d + n * unit;

            // we are sure the unit is Months or Years
            if (endOfMonth && isEndOfMonth(d))
                return Calendar<Date>::endOfMonth(d1);

            return adjust(d1, c);
        }
    }

     template <class Date>
    inline Date Calendar<Date>::advance(const Date& d,
                                 const Period& p,
                                 BusinessDayConvention c,
                                 bool endOfMonth) const {
        return advance(d, p.length(), p.units(), c, endOfMonth);
    }

      template <class Date>
    inline typename type_traits<Date>::serial_type Calendar<Date>::businessDaysBetween(
        const Date& from,
                                                                       const Date& to,
                                                                       bool includeFirst,
                                                                       bool includeLast) const {
        type_traits<Date>::serial_type wd = 0;
        if (from != to) {
            if (from < to) {
                // the last one is treated separately to avoid
                // incrementing Date::maxDate()
                for (Date d = from; d < to; ++d) {
                    if (isBusinessDay(d))
                        ++wd;
                }
                if (isBusinessDay(to))
                    ++wd;
            } else if (from > to) {
                for (Date d = to; d < from; ++d) {
                    if (isBusinessDay(d))
                        ++wd;
                }
                if (isBusinessDay(from))
                    ++wd;
            }

            if (isBusinessDay(from) && !includeFirst)
                --wd;
            if (isBusinessDay(to) && !includeLast)
                --wd;

            if (from > to)
                wd = -wd;
        } else if (includeFirst && includeLast && isBusinessDay(from)) {
            wd = 1;
        }

        return wd;
    }


    // Western calendars

      template <class Date>
    inline bool Calendar<Date>::WesternImpl::isWeekend(Weekday w) const {
        return w == Saturday || w == Sunday;
    }

     template <class Date>
    inline Day Calendar<Date>::WesternImpl::easterMonday(Year y) {
        static const Day EasterMonday[] = {
            98,  90,  103, 95,  114, 106, 91,  111, 102,      // 1901-1909
            87,  107, 99,  83,  103, 95,  115, 99,  91,  111, // 1910-1919
            96,  87,  107, 92,  112, 103, 95,  108, 100, 91,  // 1920-1929
            111, 96,  88,  107, 92,  112, 104, 88,  108, 100, // 1930-1939
            85,  104, 96,  116, 101, 92,  112, 97,  89,  108, // 1940-1949
            100, 85,  105, 96,  109, 101, 93,  112, 97,  89,  // 1950-1959
            109, 93,  113, 105, 90,  109, 101, 86,  106, 97,  // 1960-1969
            89,  102, 94,  113, 105, 90,  110, 101, 86,  106, // 1970-1979
            98,  110, 102, 94,  114, 98,  90,  110, 95,  86,  // 1980-1989
            106, 91,  111, 102, 94,  107, 99,  90,  103, 95,  // 1990-1999
            115, 106, 91,  111, 103, 87,  107, 99,  84,  103, // 2000-2009
            95,  115, 100, 91,  111, 96,  88,  107, 92,  112, // 2010-2019
            104, 95,  108, 100, 92,  111, 96,  88,  108, 92,  // 2020-2029
            112, 104, 89,  108, 100, 85,  105, 96,  116, 101, // 2030-2039
            93,  112, 97,  89,  109, 100, 85,  105, 97,  109, // 2040-2049
            101, 93,  113, 97,  89,  109, 94,  113, 105, 90,  // 2050-2059
            110, 101, 86,  106, 98,  89,  102, 94,  114, 105, // 2060-2069
            90,  110, 102, 86,  106, 98,  111, 102, 94,  114, // 2070-2079
            99,  90,  110, 95,  87,  106, 91,  111, 103, 94,  // 2080-2089
            107, 99,  91,  103, 95,  115, 107, 91,  111, 103, // 2090-2099
            88,  108, 100, 85,  105, 96,  109, 101, 93,  112, // 2100-2109
            97,  89,  109, 93,  113, 105, 90,  109, 101, 86,  // 2110-2119
            106, 97,  89,  102, 94,  113, 105, 90,  110, 101, // 2120-2129
            86,  106, 98,  110, 102, 94,  114, 98,  90,  110, // 2130-2139
            95,  86,  106, 91,  111, 102, 94,  107, 99,  90,  // 2140-2149
            103, 95,  115, 106, 91,  111, 103, 87,  107, 99,  // 2150-2159
            84,  103, 95,  115, 100, 91,  111, 96,  88,  107, // 2160-2169
            92,  112, 104, 95,  108, 100, 92,  111, 96,  88,  // 2170-2179
            108, 92,  112, 104, 89,  108, 100, 85,  105, 96,  // 2180-2189
            116, 101, 93,  112, 97,  89,  109, 100, 85,  105  // 2190-2199
        };
        return EasterMonday[y - 1901];
    }

    // Orthodox calendars

     template <class Date>
    inline bool Calendar<Date>::OrthodoxImpl::isWeekend(Weekday w) const {
        return w == Saturday || w == Sunday;
    }

     template <class Date>
    inline Day Calendar<Date>::OrthodoxImpl::easterMonday(Year y) {
        static const Day EasterMonday[] = {
            105, 118, 110, 102, 121, 106, 126, 118, 102,      // 1901-1909
            122, 114, 99,  118, 110, 95,  115, 106, 126, 111, // 1910-1919
            103, 122, 107, 99,  119, 110, 123, 115, 107, 126, // 1920-1929
            111, 103, 123, 107, 99,  119, 104, 123, 115, 100, // 1930-1939
            120, 111, 96,  116, 108, 127, 112, 104, 124, 115, // 1940-1949
            100, 120, 112, 96,  116, 108, 128, 112, 104, 124, // 1950-1959
            109, 100, 120, 105, 125, 116, 101, 121, 113, 104, // 1960-1969
            117, 109, 101, 120, 105, 125, 117, 101, 121, 113, // 1970-1979
            98,  117, 109, 129, 114, 105, 125, 110, 102, 121, // 1980-1989
            106, 98,  118, 109, 122, 114, 106, 118, 110, 102, // 1990-1999
            122, 106, 126, 118, 103, 122, 114, 99,  119, 110, // 2000-2009
            95,  115, 107, 126, 111, 103, 123, 107, 99,  119, // 2010-2019
            111, 123, 115, 107, 127, 111, 103, 123, 108, 99,  // 2020-2029
            119, 104, 124, 115, 100, 120, 112, 96,  116, 108, // 2030-2039
            128, 112, 104, 124, 116, 100, 120, 112, 97,  116, // 2040-2049
            108, 128, 113, 104, 124, 109, 101, 120, 105, 125, // 2050-2059
            117, 101, 121, 113, 105, 117, 109, 101, 121, 105, // 2060-2069
            125, 110, 102, 121, 113, 98,  118, 109, 129, 114, // 2070-2079
            106, 125, 110, 102, 122, 106, 98,  118, 110, 122, // 2080-2089
            114, 99,  119, 110, 102, 115, 107, 126, 118, 103, // 2090-2099
            123, 115, 100, 120, 112, 96,  116, 108, 128, 112, // 2100-2109
            104, 124, 109, 100, 120, 105, 125, 116, 108, 121, // 2110-2119
            113, 104, 124, 109, 101, 120, 105, 125, 117, 101, // 2120-2129
            121, 113, 98,  117, 109, 129, 114, 105, 125, 110, // 2130-2139
            102, 121, 113, 98,  118, 109, 129, 114, 106, 125, // 2140-2149
            110, 102, 122, 106, 126, 118, 103, 122, 114, 99,  // 2150-2159
            119, 110, 102, 115, 107, 126, 111, 103, 123, 114, // 2160-2169
            99,  119, 111, 130, 115, 107, 127, 111, 103, 123, // 2170-2179
            108, 99,  119, 104, 124, 115, 100, 120, 112, 103, // 2180-2189
            116, 108, 128, 119, 104, 124, 116, 100, 120, 112  // 2190-2199
        };
        return EasterMonday[y - 1901];
    }

    // std::vector<Date> Calendar<Date>::holidayList(const Calendar<Date>& calendar,
    //    const Date& from, const Date& to, bool includeWeekEnds) {

    //    QL_REQUIRE(to>from, "'from' date ("
    //        << from << ") must be earlier than 'to' date ("
    //        << to << ")");
    //    std::vector<Date> result;
    //    for (Date d = from; d <= to; ++d) {
    //        if (calendar.isHoliday(d)
    //            && (includeWeekEnds || !calendar.isWeekend(d.weekday())))
    //            result.push_back(d);
    //   }
    //   return result;
    //}

     template <class Date>
    inline std::vector<Date>
    Calendar<Date>::holidayList(const Date& from, const Date& to, bool includeWeekEnds) const {

        QL_REQUIRE(to > from, "'from' date ({}) must be earlier than 'to' date ({})", from, to);
        std::vector<Date> result;
        for (Date d = from; d <= to; ++d) {
            if (isHoliday(d) && (includeWeekEnds || !isWeekend(d.weekday())))
                result.push_back(d);
        }
        return result;
    }

     template <class Date>
    inline std::vector<Date> Calendar<Date>::businessDayList(const Date& from,
                                                             const Date& to) const {

        QL_REQUIRE(to > from, "'from' date ({}) must be earlier than 'to' date ({})", from, to);
        std::vector<Date> result;
        for (Date d = from; d <= to; ++d) {
            if (isBusinessDay(d))
                result.push_back(d);
        }
        return result;
    }

}

#endif
