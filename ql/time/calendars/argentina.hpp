/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2005 StatPro Italia srl

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

/*! \file argentina.hpp
    \brief Argentinian calendars
*/

#ifndef quantlib_argentinian_calendar_hpp
#define quantlib_argentinian_calendar_hpp

#include "calendar.hpp"

namespace QuantLib {

    //! Argentinian calendars
    /*! Holidays for the Buenos Aires stock exchange
        (data from <http://www.merval.sba.com.ar/>):
        <ul>
        <li>Saturdays</li>
        <li>Sundays</li>
        <li>New Year's Day, January 1st</li>
        <li>Holy Thursday</li>
        <li>Good Friday</li>
        <li>Labour Day, May 1st</li>
        <li>May Revolution, May 25th</li>
        <li>Death of General Manuel Belgrano, third Monday of June</li>
        <li>Independence Day, July 9th</li>
        <li>Death of General José de San Martín, third Monday of August</li>
        <li>Columbus Day, October 12th (moved to preceding Monday if
            on Tuesday or Wednesday and to following if on Thursday
            or Friday)</li>
        <li>Immaculate Conception, December 8th</li>
        <li>Christmas Eve, December 24th</li>
        <li>New Year's Eve, December 31th</li>
        </ul>

        \ingroup calendars
    */
    template <class Date>
    class Argentina : public Calendar<Date> {
      private:
        class MervalImpl : public Calendar<Date>::WesternImpl {
          public:
            std::string name() const { return "Buenos Aires stock exchange"; }
            bool isBusinessDay(const Date&) const;
        };
      public:
        enum Market { Merval   //!< Buenos Aires stock exchange calendar
        };
        Argentina(Market m = Merval) {
            // all calendar instances share the same implementation instance
            static std::shared_ptr<Calendar<Date>::Impl> impl(new Argentina::MervalImpl);
            impl_ = impl;
        }
    };

template <class Date>
inline
    bool Argentina<Date>::MervalImpl::isBusinessDay(const Date& date) const {
        Weekday w = date_traits<Date>::weekday(date);
        Day d = date_traits<Date>::dayOfMonth(date);
        Day dd = date_traits<Date>::dayOfYear(date);
        Month m = date_traits<Date>::month(date);
        Year y = date.year();
        Day em = easterMonday(y);
        if (isWeekend(w)
            // New Year's Day
            || (d == 1 && m == January)
            // Holy Thursday
            || (dd == em-4)
            // Good Friday
            || (dd == em-3)
            // Labour Day
            || (d == 1 && m == May)
            // May Revolution
            || (d == 25 && m == May)
            // Death of General Manuel Belgrano
            || (d >= 15 && d <= 21 && w == Monday && m == June)
            // Independence Day
            || (d == 9 && m == July)
            // Death of General José de San Martín
            || (d >= 15 && d <= 21 && w == Monday && m == August)
            // Columbus Day
            || ((d == 10 || d == 11 || d == 12 || d == 15 || d == 16)
                && w == Monday && m == October)
            // Immaculate Conception
            || (d == 8 && m == December)
            // Christmas Eve
            || (d == 24 && m == December)
            // New Year's Eve
            || ((d == 31 || (d == 30 && w == Friday)) && m == December))
            return false; // NOLINT(readability-simplify-boolean-expr)
        return true;
    }
}


#endif
