/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2004 Ferdinando Ametrano
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

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

/*! \file austria.hpp
    \brief Austrian calendars
*/

#ifndef quantlib_austria_calendar_hpp
#define quantlib_austria_calendar_hpp

#include "calendar.hpp"

namespace QuantLib {

    //! Austrian calendars
    /*! Public holidays:
        <ul>
        <li>Saturdays</li>
        <li>Sundays</li>
        <li>New Year's Day, January 1st</li>
        <li>Epiphany, January 6th</li>
        <li>Easter Monday</li>
        <li>Ascension Thursday</li>
        <li>Whit Monday</li>
        <li>Corpus Christi</li>
        <li>Labour Day, May 1st</li>
        <li>Assumption Day, August 15th</li>
        <li>National Holiday, October 26th, since 1967</li>
        <li>All Saints Day, November 1st</li>
        <li>National Holiday, November 12th, 1919-1934</li>
        <li>Immaculate Conception Day, December 8th</li>
        <li>Christmas, December 25th</li>
        <li>St. Stephen, December 26th</li>
        </ul>

        Holidays for the stock exchange (data from https://www.wienerborse.at/en/trading/trading-information/trading-calendar/):
        <ul>
        <li>Saturdays</li>
        <li>Sundays</li>
        <li>New Year's Day, January 1st</li>
        <li>Good Friday</li>
        <li>Easter Monday</li>
        <li>Whit Monday</li>
        <li>Labour Day, May 1st</li>
        <li>National Holiday, October 26th, since 1967</li>
        <li>National Holiday, November 12th, 1919-1934</li>
        <li>Christmas Eve, December 24th</li>
        <li>Christmas, December 25th</li>
        <li>St. Stephen, December 26th</li>
        <li>Exchange Holiday</li>
        </ul>

        \ingroup calendars
    */
    template <class Date>
    class Austria : public Calendar<Date> {
      private:
        class SettlementImpl : public Calendar<Date>::WesternImpl {
          public:
            std::string name() const { return "Austrian settlement"; }
            bool isBusinessDay(const Date&) const;
        };
        class ExchangeImpl : public Calendar<Date>::WesternImpl {
          public:
            std::string name() const { return "Vienna stock exchange"; }
            bool isBusinessDay(const Date&) const;
        };
      public:
        //! Austrian calendars
        enum Market { Settlement,     //!< generic settlement calendar
                      Exchange        //!< Paris stock-exchange calendar
        };
        explicit Austria(Market market = Settlement);
    };


        template <class Date>
    inline Austria<Date>::Austria(Austria<Date>::Market market) {
        // all calendar instances on the same market share the same
        // implementation instance
        static std::shared_ptr<Calendar<Date>::Impl> settlementImpl(new Austria<Date>::SettlementImpl);
        static std::shared_ptr<Calendar<Date>::Impl> exchangeImpl(new Austria<Date>::ExchangeImpl);
        switch (market) {
            case Settlement:
                impl_ = settlementImpl;
                break;
            case Exchange:
                impl_ = exchangeImpl;
                break;
            default:
                QL_FAIL("unknown market");
        }
    }


        template <class Date>
    inline bool Austria<Date>::SettlementImpl::isBusinessDay(const Date& date) const {
        Weekday w = date_traits<Date>::weekday(date);
        Day d = date_traits<Date>::dayOfMonth(date);
        Day dd = date_traits<Date>::dayOfYear(date);
        Month m = date_traits<Date>::ql_month(date);
        Year y = date.year();
        Day em = easterMonday(y);
        if (isWeekend(w)
            // New Year's Day
            || (d == 1 && m == January)
            // Epiphany
            || (d == 6 && m == January)
            // Easter Monday
            || (dd == em)
            // Ascension Thurday
            || (dd == em + 38)
            // Whit Monday
            || (dd == em + 49)
            // Corpus Christi
            || (dd == em + 59)
            // Labour Day
            || (d == 1 && m == May)
            // Assumption
            || (d == 15 && m == August)
            // National Holiday since 1967
            || (d == 26 && m == October && y >= 1967)
            // National Holiday 1919-1934
            || (d == 12 && m == November && y >= 1919 && y <= 1934)
            // All Saints' Day
            || (d == 1 && m == November)
            // Immaculate Conception
            || (d == 8 && m == December)
            // Christmas
            || (d == 25 && m == December)
            // St. Stephen
            || (d == 26 && m == December))
            return false; // NOLINT(readability-simplify-boolean-expr)
        return true;
    }


            template <class Date>
    inline bool Austria<Date>::ExchangeImpl::isBusinessDay(const Date& date) const {
        Weekday w = date_traits<Date>::weekday(date);
        Day d = date_traits<Date>::dayOfMonth(date);
        Day dd = date_traits<Date>::dayOfYear(date);
        Month m = date_traits<Date>::ql_month(date);
        Year y = date.year();
        Day em = easterMonday(y);
        if (isWeekend(w)
            // New Year's Day
            || (d == 1 && m == January)
            // Good Friday
            || (dd == em - 3)
            // Easter Monday
            || (dd == em)
            // Whit Monay
            || (dd == em + 49)
            // Labour Day
            || (d == 1 && m == May)
            // National Holiday since 1967
            || (d == 26 && m == October && y >= 1967)
            // National Holiday 1919-1934
            || (d == 12 && m == November && y >= 1919 && y <= 1934)
            // Christmas' Eve
            || (d == 24 && m == December)
            // Christmas
            || (d == 25 && m == December)
            // St. Stephen
            || (d == 26 && m == December)
            // Exchange Holiday
            || (d == 31 && m == December))
            return false; // NOLINT(readability-simplify-boolean-expr)
        return true;
    }

}


#endif
