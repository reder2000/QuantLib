/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2007 StatPro Italia srl

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

#include <ql/time/calendars/canada.hpp>
#include "../ql_errors.hpp"

namespace QuantLib {
    template <class ExtDate> inline
    Canada<ExtDate>::Canada(Canada<ExtDate>::Market market) {
        // all calendar instances share the same implementation instance
        static std::shared_ptr<Calendar<ExtDate>::Impl> settlementImpl(
                                                  new Canada<ExtDate>::SettlementImpl);
        static std::shared_ptr<Calendar<ExtDate>::Impl> tsxImpl(new Canada<ExtDate>::TsxImpl);
        switch (market) {
          case Settlement:
            this->impl_ = settlementImpl;
            break;
          case TSX:
            this->impl_ = tsxImpl;
            break;
          default:
            QL_FAIL("unknown market");
        }
    }
    template <class ExtDate> inline
    bool Canada<ExtDate>::SettlementImpl::isBusinessDay(const ExtDate& date) const {
        Weekday w = date.weekday();
        Day d = date.dayOfMonth(), dd = date.dayOfYear();
        Month m = date.month();
        Year y = date.year();
        Day em = this->easterMonday(y);
        if (this->isWeekend(w)
            // New Year's Day (possibly moved to Monday)
            || ((d == 1 || ((d == 2 || d == 3) && w == Monday)) && m == January)
            // Family Day (third Monday in February, since 2008)
            || ((d >= 15 && d <= 21) && w == Monday && m == February
                && y >= 2008)
            // Good Friday
            || (dd == em-3)
            // The Monday on or preceding 24 May (Victoria Day)
            || (d > 17 && d <= 24 && w == Monday && m == May)
            // July 1st, possibly moved to Monday (Canada Day)
            || ((d == 1 || ((d == 2 || d == 3) && w == Monday)) && m==July)
            // first Monday of August (Provincial Holiday)
            || (d <= 7 && w == Monday && m == August)
            // first Monday of September (Labor Day)
            || (d <= 7 && w == Monday && m == September)
            // second Monday of October (Thanksgiving Day)
            || (d > 7 && d <= 14 && w == Monday && m == October)
            // November 11th (possibly moved to Monday)
            || ((d == 11 || ((d == 12 || d == 13) && w == Monday))
                && m == November)
            // Christmas (possibly moved to Monday or Tuesday)
            || ((d == 25 || (d == 27 && (w == Monday || w == Tuesday)))
                && m == December)
            // Boxing Day (possibly moved to Monday or Tuesday)
            || ((d == 26 || (d == 28 && (w == Monday || w == Tuesday)))
                && m == December)
            )
            return false; // NOLINT(readability-simplify-boolean-expr)
        return true;
    }
    template <class ExtDate> inline
    bool Canada<ExtDate>::TsxImpl::isBusinessDay(const ExtDate& date) const {
        Weekday w = date.weekday();
        Day d = date.dayOfMonth(), dd = date.dayOfYear();
        Month m = date.month();
        Year y = date.year();
        Day em = this->easterMonday(y);
        if (this->isWeekend(w)
            // New Year's Day (possibly moved to Monday)
            || ((d == 1 || ((d == 2 || d == 3) && w == Monday)) && m == January)
            // Family Day (third Monday in February, since 2008)
            || ((d >= 15 && d <= 21) && w == Monday && m == February
                && y >= 2008)
            // Good Friday
            || (dd == em-3)
            // The Monday on or preceding 24 May (Victoria Day)
            || (d > 17 && d <= 24 && w == Monday && m == May)
            // July 1st, possibly moved to Monday (Canada Day)
            || ((d == 1 || ((d == 2 || d == 3) && w == Monday)) && m==July)
            // first Monday of August (Provincial Holiday)
            || (d <= 7 && w == Monday && m == August)
            // first Monday of September (Labor Day)
            || (d <= 7 && w == Monday && m == September)
            // second Monday of October (Thanksgiving Day)
            || (d > 7 && d <= 14 && w == Monday && m == October)
            // Christmas (possibly moved to Monday or Tuesday)
            || ((d == 25 || (d == 27 && (w == Monday || w == Tuesday)))
                && m == December)
            // Boxing Day (possibly moved to Monday or Tuesday)
            || ((d == 26 || (d == 28 && (w == Monday || w == Tuesday)))
                && m == December)
            )
            return false; // NOLINT(readability-simplify-boolean-expr)
        return true;
    }

}
