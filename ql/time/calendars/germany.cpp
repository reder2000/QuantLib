/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2004 Ferdinando Ametrano

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

#include <ql/time/calendars/germany.hpp>
#include "../ql_errors.hpp"

namespace QuantLib {
    template <class ExtDate> inline
    Germany<ExtDate>::Germany(Germany<ExtDate>::Market market) {
        // all calendar instances on the same market share the same
        // implementation instance
        static std::shared_ptr<typename Calendar<ExtDate>::Impl> settlementImpl(
            new Germany<ExtDate>::SettlementImpl);
        static std::shared_ptr<typename Calendar<ExtDate>::Impl> frankfurtStockExchangeImpl(
            new Germany<ExtDate>::FrankfurtStockExchangeImpl);
        static std::shared_ptr<typename Calendar<ExtDate>::Impl> xetraImpl(
            new Germany<ExtDate>::XetraImpl);
        static std::shared_ptr<typename Calendar<ExtDate>::Impl> eurexImpl(
            new Germany<ExtDate>::EurexImpl);
        static std::shared_ptr<typename Calendar<ExtDate>::Impl> euwaxImpl(
            new Germany<ExtDate>::EuwaxImpl);

        switch (market) {
          case Settlement:
            this->impl_ = settlementImpl;
            break;
          case FrankfurtStockExchange:
            this->impl_ = frankfurtStockExchangeImpl;
            break;
          case Xetra:
            this->impl_ = xetraImpl;
            break;
          case Eurex:
            this->impl_ = eurexImpl;
            break;
          case Euwax:
            this->impl_ = euwaxImpl;
            break;
          default:
            QL_FAIL("unknown market");
        }
    }

    template <class ExtDate> inline
    bool Germany<ExtDate>::SettlementImpl::isBusinessDay(const ExtDate& dat) const {
        auto date = to_DateLike(dat);
        auto sn = date.serialNumber();
        Weekday w = date.weekday(sn);
        Day d = date.dayOfMonth(sn), dd = date.dayOfYear(sn);
        Month m = date.month(sn);
        Year y = date.year(sn);
        Day em = this->easterMonday(y);
        if (this->isWeekend(w)
            // New Year's Day
            || (d == 1 && m == January)
            // Good Friday
            || (dd == em-3)
            // Easter Monday
            || (dd == em)
            // Ascension Thursday
            || (dd == em+38)
            // Whit Monday
            || (dd == em+49)
            // Corpus Christi
            || (dd == em+59)
            // Labour Day
            || (d == 1 && m == May)
            // National Day
            || (d == 3 && m == October)
            // Christmas Eve
            || (d == 24 && m == December)
            // Christmas
            || (d == 25 && m == December)
            // Boxing Day
            || (d == 26 && m == December))
            return false; // NOLINT(readability-simplify-boolean-expr)
        return true;
    }
    template <class ExtDate> inline
    bool Germany<ExtDate>::FrankfurtStockExchangeImpl::isBusinessDay(
      const ExtDate& dat) const {
        auto date = to_DateLike(dat);
        auto sn = date.serialNumber();
        Weekday w = date.weekday(sn);
        Day d = date.dayOfMonth(sn), dd = date.dayOfYear(sn);
        Month m = date.month(sn);
        Year y = date.year(sn);
        Day em = this->easterMonday(y);
        if (this->isWeekend(w)
            // New Year's Day
            || (d == 1 && m == January)
            // Good Friday
            || (dd == em-3)
            // Easter Monday
            || (dd == em)
            // Labour Day
            || (d == 1 && m == May)
            // Christmas' Eve
            || (d == 24 && m == December)
            // Christmas
            || (d == 25 && m == December)
            // Christmas Day
            || (d == 26 && m == December))
            return false; // NOLINT(readability-simplify-boolean-expr)
        return true;
    }
    template <class ExtDate> inline
    bool Germany<ExtDate>::XetraImpl::isBusinessDay(const ExtDate& dat) const {
        auto date = to_DateLike(dat);
        auto sn = date.serialNumber();
        Weekday w = date.weekday(sn);
        Day d = date.dayOfMonth(sn), dd = date.dayOfYear(sn);
        Month m = date.month(sn);
        Year y = date.year(sn);
        Day em = this->easterMonday(y);
        if (this->isWeekend(w)
            // New Year's Day
            || (d == 1 && m == January)
            // Good Friday
            || (dd == em-3)
            // Easter Monday
            || (dd == em)
            // Labour Day
            || (d == 1 && m == May)
            // Christmas' Eve
            || (d == 24 && m == December)
            // Christmas
            || (d == 25 && m == December)
            // Christmas Day
            || (d == 26 && m == December))
            return false; // NOLINT(readability-simplify-boolean-expr)
        return true;
    }
    template <class ExtDate> inline
    bool Germany<ExtDate>::EurexImpl::isBusinessDay(const ExtDate& dat) const {
        auto date = to_DateLike(dat);
        auto sn = date.serialNumber();
        Weekday w = date.weekday(sn);
        Day d = date.dayOfMonth(sn), dd = date.dayOfYear(sn);
        Month m = date.month(sn);
        Year y = date.year(sn);
        Day em = this->easterMonday(y);
        if (this->isWeekend(w)
            // New Year's Day
            || (d == 1 && m == January)
            // Good Friday
            || (dd == em-3)
            // Easter Monday
            || (dd == em)
            // Labour Day
            || (d == 1 && m == May)
            // Christmas' Eve
            || (d == 24 && m == December)
            // Christmas
            || (d == 25 && m == December)
            // Christmas Day
            || (d == 26 && m == December)
            // New Year's Eve
            || (d == 31 && m == December))
            return false; // NOLINT(readability-simplify-boolean-expr)
        return true;
    }
    template <class ExtDate> inline
    bool Germany<ExtDate>::EuwaxImpl::isBusinessDay(const ExtDate& dat) const {
        auto date = to_DateLike(dat);
        auto sn = date.serialNumber();
        Weekday w = date.weekday(sn);
        Day d = date.dayOfMonth(sn), dd = date.dayOfYear(sn);
        Month m = date.month(sn);
        Year y = date.year(sn);
        Day em = this->easterMonday(y);
        if ((w == Saturday || w == Sunday)
            // New Year's Day
            || (d == 1 && m == January)
            // Good Friday
            || (dd == em-3)
            // Easter Monday
            || (dd == em)
            // Labour Day
            || (d == 1 && m == May)
            // Whit Monday
            || (dd == em+49)
            // Christmas' Eve
            || (d == 24 && m == December)
            // Christmas
            || (d == 25 && m == December)
            // Christmas Day
            || (d == 26 && m == December))
            return false; // NOLINT(readability-simplify-boolean-expr)
        return true;
    }
}

