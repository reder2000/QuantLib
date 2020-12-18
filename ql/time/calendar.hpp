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
#pragma once
#ifndef quantlib_calendar_hpp
#define quantlib_calendar_hpp

#include "ql_errors.hpp"
#include <ql/time/date.hpp>
#include <ql/time/businessdayconvention.hpp>
//#include <ql/shared_ptr.hpp>
#include <set>
#include <vector>
#include <string>

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
    template <class ExtDate=Date>
    class Calendar {
      protected:
        //! abstract base class for calendar implementations
        class Impl {
          public:
            virtual ~Impl() {}
            virtual std::string name() const = 0;
            virtual bool isBusinessDay(const ExtDate&) const = 0;
            virtual bool isWeekend(Weekday) const = 0;
            std::set<ExtDate> addedHolidays, removedHolidays;
        };
        std::shared_ptr<Impl> impl_;
      public:
        /*! The default constructor returns a calendar with a null
            implementation, which is therefore unusable except as a
            placeholder.
        */
        Calendar() {}
        //! \name Calendar interface
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
        const std::set<ExtDate>& addedHolidays() const;
        
        /*! Returns the set of removed holidays for the given calendar */
        const std::set<ExtDate>& removedHolidays() const;

        bool isBusinessDay(const ExtDate& d) const;
        /*! Returns <tt>true</tt> iff the date is a holiday for the given
            market.
        */
        bool isHoliday(const ExtDate& d) const;
        /*! Returns <tt>true</tt> iff the weekday is part of the
            weekend for the given market.
        */
        bool isWeekend(Weekday w) const;
        /*! Returns <tt>true</tt> iff in the given market, the date is on
            or after the last business day for that month.
        */
        bool isEndOfMonth(const ExtDate& d) const;
        //! last business day of the month to which the given date belongs
        ExtDate endOfMonth(const ExtDate& d) const;

        /*! Adds a date to the set of holidays for the given calendar. */
        void addHoliday(const ExtDate&);
        /*! Removes a date from the set of holidays for the given calendar. */
        void removeHoliday(const ExtDate&);

        /*! Returns the holidays between two dates.

            \deprecated Use the non-static overload.
                        Deprecated in version 1.18.
        */
        QL_DEPRECATED
        static std::vector<ExtDate> holidayList(const Calendar<ExtDate>& calendar,
                                             const ExtDate& from,
                                             const ExtDate& to,
                                             bool includeWeekEnds = false);

        /*! Returns the holidays between two dates. */
        std::vector<ExtDate> holidayList(const ExtDate& from,
                                      const ExtDate& to,
                                      bool includeWeekEnds = false) const;
        /*! Returns the business days between two dates. */
        std::vector<ExtDate> businessDayList(const ExtDate& from,
                                          const ExtDate& to) const;

        /*! Adjusts a non-business day to the appropriate near business day
            with respect to the given convention.
        */
        ExtDate adjust(const ExtDate&,
                    BusinessDayConvention convention = Following) const;
        /*! Advances the given date of the given number of business days and
            returns the result.
            \note The input date is not modified.
        */
        ExtDate advance(const ExtDate&,
                     Integer n,
                     TimeUnit unit,
                     BusinessDayConvention convention = Following,
                     bool endOfMonth = false) const;
        /*! Advances the given date as specified by the given period and
            returns the result.
            \note The input date is not modified.
        */
        ExtDate advance(const ExtDate& date,
                     const Period& period,
                     BusinessDayConvention convention = Following,
                     bool endOfMonth = false) const;
        /*! Calculates the number of business days between two given
            dates and returns the result.
        */
        serial_type businessDaysBetween(const ExtDate& from,
                                              const ExtDate& to,
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
        \relates Calendar
    */
    template <class ExtDate>
    bool operator==(const Calendar<ExtDate>&, const Calendar<ExtDate>&);

    /*! \relates Calendar */ template <class ExtDate>
    bool operator!=(const Calendar<ExtDate>&, const Calendar<ExtDate>&);

    /*! \relates Calendar */ template <class ExtDate>
    std::ostream& operator<<(std::ostream&, const Calendar<ExtDate>&);


    // inline definitions

    template <class ExtDate>
    inline bool Calendar<ExtDate>::empty() const {
        return !impl_;
    }

    template <class ExtDate> inline  std::string Calendar<ExtDate>::name() const {
        QL_REQUIRE(impl_, "no calendar implementation provided");
        return impl_->name();
    }

    template <class ExtDate> inline  const std::set<ExtDate>& Calendar<ExtDate>::addedHolidays() const {
        QL_REQUIRE(impl_, "no calendar implementation provided");

        return impl_->addedHolidays;
    }

    template <class ExtDate> inline  const std::set<ExtDate>& Calendar<ExtDate>::removedHolidays() const {
        QL_REQUIRE(impl_, "no calendar implementation provided");

        return impl_->removedHolidays;
    }

    template <class ExtDate> inline  bool Calendar<ExtDate>::isBusinessDay(const ExtDate& d) const {
        QL_REQUIRE(impl_, "no calendar implementation provided");

#ifdef QL_HIGH_RESOLUTION_DATE
        const ExtDate _d(d.dayOfMonth(), d.month(), d.year());
#else
        const ExtDate& _d = d;
#endif

        if (!impl_->addedHolidays.empty() &&
            impl_->addedHolidays.find(_d) != impl_->addedHolidays.end())
            return false;

        if (!impl_->removedHolidays.empty() &&
            impl_->removedHolidays.find(_d) != impl_->removedHolidays.end())
            return true;

        return impl_->isBusinessDay(_d);
    }

    template <class ExtDate> inline  bool Calendar<ExtDate>::isEndOfMonth(const ExtDate& d) const {
        return (d.month() != adjust(d+1).month());
    }

    template <class ExtDate> inline  ExtDate Calendar<ExtDate>::endOfMonth(const ExtDate& d) const {
        return adjust(ExtDate::endOfMonth(d), Preceding);
    }

    template <class ExtDate> inline  bool Calendar<ExtDate>::isHoliday(const ExtDate& d) const {
        return !isBusinessDay(d);
    }

    template <class ExtDate> inline  bool Calendar<ExtDate>::isWeekend(Weekday w) const {
        QL_REQUIRE(impl_, "no calendar implementation provided");
        return impl_->isWeekend(w);
    }

    template <class ExtDate> inline  bool operator==(const Calendar<ExtDate>& c1, const Calendar<ExtDate>& c2) {
        return (c1.empty() && c2.empty())
            || (!c1.empty() && !c2.empty() && c1.name() == c2.name());
    }

    template <class ExtDate> inline  bool operator!=(const Calendar<ExtDate>& c1, const Calendar<ExtDate>& c2) {
        return !(c1 == c2);
    }

    template <class ExtDate> inline  std::ostream& operator<<(std::ostream& out, const Calendar<ExtDate> &c) {
        return out << c.name();
    }

}
#include "calendar.cpp"
#endif
