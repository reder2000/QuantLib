/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2004 StatPro Italia srl

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

#include <ql/time/calendars/italy.hpp>
#include "../ql_errors.hpp"

namespace QuantLib {
    template <class ExtDate> inline
    Italy<ExtDate>::Italy(Italy<ExtDate>::Market market) {
        // all calendar instances on the same market share the same
        // implementation instance
        static std::shared_ptr<typename Calendar<ExtDate>::Impl> settlementImpl(
                                                   new Italy<ExtDate>::SettlementImpl);
        static std::shared_ptr<typename Calendar<ExtDate>::Impl> exchangeImpl(
                                                   new Italy<ExtDate>::ExchangeImpl);
        switch (market) {
          case Settlement:
            this->impl_ = settlementImpl;
            break;
          case Exchange:
            this->impl_ = exchangeImpl;
            break;
          default:
            QL_FAIL("unknown market");
        }
    }

    template <class ExtDate> inline
    bool Italy<ExtDate>::SettlementImpl::isBusinessDay(const ExtDate& dat) const {
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
            // Epiphany
            || (d == 6 && m == January)
            // Easter Monday
            || (dd == em)
            // Liberation Day
            || (d == 25 && m == April)
            // Labour Day
            || (d == 1 && m == May)
            // Republic Day
            || (d == 2 && m == June && y >= 2000)
            // Assumption
            || (d == 15 && m == August)
            // All Saints' Day
            || (d == 1 && m == November)
            // Immaculate Conception
            || (d == 8 && m == December)
            // Christmas
            || (d == 25 && m == December)
            // St. Stephen
            || (d == 26 && m == December)
            // December 31st, 1999 only
            || (d == 31 && m == December && y == 1999))
            return false; // NOLINT(readability-simplify-boolean-expr)
        return true;
    }

    template <class ExtDate> inline
    bool Italy<ExtDate>::ExchangeImpl::isBusinessDay(const ExtDate& dat) const {
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
            // Assumption
            || (d == 15 && m == August)
            // Christmas' Eve
            || (d == 24 && m == December)
            // Christmas
            || (d == 25 && m == December)
            // St. Stephen
            || (d == 26 && m == December)
            // New Year's Eve
            || (d == 31 && m == December))
            return false; // NOLINT(readability-simplify-boolean-expr)
        return true;
    }

}

