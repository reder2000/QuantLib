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

#include <ql/time/calendars/unitedkingdom.hpp>
#include "../ql_errors.hpp"

namespace QuantLib {
    template <class ExtDate> inline
    UnitedKingdom<ExtDate>::UnitedKingdom(UnitedKingdom<ExtDate>::Market market) {
        // all calendar instances on the same market share the same
        // implementation instance
        static std::shared_ptr<typename Calendar<ExtDate>::Impl> settlementImpl(
                                           new UnitedKingdom<ExtDate>::SettlementImpl);
        static std::shared_ptr<typename Calendar<ExtDate>::Impl> exchangeImpl(
                                           new UnitedKingdom<ExtDate>::ExchangeImpl);
        static std::shared_ptr<typename Calendar<ExtDate>::Impl> metalsImpl(
                                           new UnitedKingdom<ExtDate>::MetalsImpl);
        switch (market) {
          case Settlement:
            this->impl_ = settlementImpl;
            break;
          case Exchange:
            this->impl_ = exchangeImpl;
            break;
          case Metals:
            this->impl_ = metalsImpl;
            break;
          default:
            QL_FAIL("unknown market");
        }
    }
    template <class ExtDate> inline
    bool UnitedKingdom<ExtDate>::SettlementImpl::isBusinessDay(const ExtDate& edate) const {
        auto& date = to_DateLike(edate);
        auto sn = date.serialNumber();
        Weekday w = date.weekday(sn);
        Day d = date.dayOfMonth(sn), dd = date.dayOfYear(sn);
        Month m = date.month(sn);
        Year y = date.year(sn);
        Day em = this->easterMonday(y);
        if (this->isWeekend(w)
            // New Year's Day (possibly moved to Monday)
            || ((d == 1 || ((d == 2 || d == 3) && w == Monday)) &&
                m == January)
            // Good Friday
            || (dd == em-3)
            // Easter Monday
            || (dd == em)
            // first Monday of May (Early May Bank Holiday)
            // moved to May 8th in 1995 and 2020 for V.E. day
            || (d <= 7 && w == Monday && m == May && y != 1995 && y != 2020)
            || (d == 8 && m == May && (y == 1995 || y == 2020))
            // last Monday of May (Spring Bank Holiday)
            || (d >= 25 && w == Monday && m == May && y != 2002 && y != 2012)
            // last Monday of August (Summer Bank Holiday)
            || (d >= 25 && w == Monday && m == August)
            // Christmas (possibly moved to Monday or Tuesday)
            || ((d == 25 || (d == 27 && (w == Monday || w == Tuesday)))
                && m == December)
            // Boxing Day (possibly moved to Monday or Tuesday)
            || ((d == 26 || (d == 28 && (w == Monday || w == Tuesday)))
                && m == December)
            // June 3rd, 2002 only (Golden Jubilee Bank Holiday)
            // June 4rd, 2002 only (special Spring Bank Holiday)
            || ((d == 3 || d == 4) && m == June && y == 2002)
            // April 29th, 2011 only (Royal Wedding Bank Holiday)
            || (d == 29 && m == April && y == 2011)
            // June 4th, 2012 only (Diamond Jubilee Bank Holiday)
            // June 5th, 2012 only (Special Spring Bank Holiday)
            || ((d == 4 || d == 5) && m == June && y == 2012)
            // December 31st, 1999 only
            || (d == 31 && m == December && y == 1999))
            return false; // NOLINT(readability-simplify-boolean-expr)
        return true;
    }

    template <class ExtDate> inline
    bool UnitedKingdom<ExtDate>::ExchangeImpl::isBusinessDay(const ExtDate& edate) const {
        auto& date = to_DateLike(edate);
        auto sn = date.serialNumber();
        Weekday w = date.weekday(sn);
        Day d = date.dayOfMonth(sn), dd = date.dayOfYear(sn);
        Month m = date.month(sn);
        Year y = date.year(sn);
        Day em = this->easterMonday(y);
        if (this->isWeekend(w)
            // New Year's Day (possibly moved to Monday)
            || ((d == 1 || ((d == 2 || d == 3) && w == Monday)) &&
                m == January)
            // Good Friday
            || (dd == em-3)
            // Easter Monday
            || (dd == em)
            // first Monday of May (Early May Bank Holiday)
            // moved to May 8th in 1995 and 2020 for V.E. day
            || (d <= 7 && w == Monday && m == May && y != 1995 && y != 2020)
            || (d == 8 && m == May && (y == 1995 || y == 2020))
            // last Monday of May (Spring Bank Holiday)
            || (d >= 25 && w == Monday && m == May && y != 2002 && y != 2012)
            // last Monday of August (Summer Bank Holiday)
            || (d >= 25 && w == Monday && m == August)
            // Christmas (possibly moved to Monday or Tuesday)
            || ((d == 25 || (d == 27 && (w == Monday || w == Tuesday)))
                && m == December)
            // Boxing Day (possibly moved to Monday or Tuesday)
            || ((d == 26 || (d == 28 && (w == Monday || w == Tuesday)))
                && m == December)
            // June 3rd, 2002 only (Golden Jubilee Bank Holiday)
            // June 4rd, 2002 only (special Spring Bank Holiday)
            || ((d == 3 || d == 4) && m == June && y == 2002)
            // April 29th, 2011 only (Royal Wedding Bank Holiday)
            || (d == 29 && m == April && y == 2011)
            // June 4th, 2012 only (Diamond Jubilee Bank Holiday)
            // June 5th, 2012 only (Special Spring Bank Holiday)
            || ((d == 4 || d == 5) && m == June && y == 2012)
            // December 31st, 1999 only
            || (d == 31 && m == December && y == 1999))
            return false; // NOLINT(readability-simplify-boolean-expr)
        return true;
    }

    template <class ExtDate> inline
    bool UnitedKingdom<ExtDate>::MetalsImpl::isBusinessDay(const ExtDate& edate) const {
        auto& date = to_DateLike(edate);
        auto sn = date.serialNumber();
        Weekday w = date.weekday(sn);
        Day d = date.dayOfMonth(sn), dd = date.dayOfYear(sn);
        Month m = date.month(sn);
        Year y = date.year(sn);
        Day em = this->easterMonday(y);
        if (this->isWeekend(w)
            // New Year's Day (possibly moved to Monday)
            || ((d == 1 || ((d == 2 || d == 3) && w == Monday)) &&
                m == January)
            // Good Friday
            || (dd == em-3)
            // Easter Monday
            || (dd == em)
            // first Monday of May (Early May Bank Holiday)
            // moved to May 8th in 1995 and 2020 for V.E. day
            || (d <= 7 && w == Monday && m == May && y != 1995 && y != 2020)
            || (d == 8 && m == May && (y == 1995 || y == 2020))
            // last Monday of May (Spring Bank Holiday)
            || (d >= 25 && w == Monday && m == May && y != 2002 && y != 2012)
            // last Monday of August (Summer Bank Holiday)
            || (d >= 25 && w == Monday && m == August)
            // Christmas (possibly moved to Monday or Tuesday)
            || ((d == 25 || (d == 27 && (w == Monday || w == Tuesday)))
                && m == December)
            // Boxing Day (possibly moved to Monday or Tuesday)
            || ((d == 26 || (d == 28 && (w == Monday || w == Tuesday)))
                && m == December)
            // June 3rd, 2002 only (Golden Jubilee Bank Holiday)
            // June 4rd, 2002 only (special Spring Bank Holiday)
            || ((d == 3 || d == 4) && m == June && y == 2002)
            // April 29th, 2011 only (Royal Wedding Bank Holiday)
            || (d == 29 && m == April && y == 2011)
            // June 4th, 2012 only (Diamond Jubilee Bank Holiday)
            // June 5th, 2012 only (Special Spring Bank Holiday)
            || ((d == 4 || d == 5) && m == June && y == 2012)
            // December 31st, 1999 only
            || (d == 31 && m == December && y == 1999))
            return false; // NOLINT(readability-simplify-boolean-expr)
        return true;
    }

}

