/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
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

#include <ql/time/calendars/target.hpp>

namespace QuantLib {
    template <class ExtDate> inline
    TARGET<ExtDate>::TARGET() {
        // all calendar instances share the same implementation instance
        static std::shared_ptr<typename Calendar<ExtDate>::Impl> impl(new TARGET<ExtDate>::Impl);
        this->impl_ = impl;
    }
    template <class ExtDate> inline
    bool TARGET<ExtDate>::Impl::isBusinessDay(const ExtDate& edate) const {
        auto& date = to_DateLike(edate);
        Weekday w = date.weekday();
        Day d = date.dayOfMonth(), dd = date.dayOfYear();
        Month m = date.month();
        Year y = date.year();
        Day em = Calendar<ExtDate>::WesternImpl::easterMonday(y);
        if (this->isWeekend(w)
            // New Year's Day
            || (d == 1  && m == January)
            // Good Friday
            || (dd == em-3 && y >= 2000)
            // Easter Monday
            || (dd == em && y >= 2000)
            // Labour Day
            || (d == 1  && m == May && y >= 2000)
            // Christmas
            || (d == 25 && m == December)
            // Day of Goodwill
            || (d == 26 && m == December && y >= 2000)
            // December 31st, 1998, 1999, and 2001 only
            || (d == 31 && m == December &&
                (y == 1998 || y == 1999 || y == 2001)))
            return false; // NOLINT(readability-simplify-boolean-expr)
        return true;
    }

}

