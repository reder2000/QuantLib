/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2013 BGC Partners L.P.

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

#include <ql/time/daycounters/actual365fixed.hpp>
#include <cmath>

namespace QuantLib {
    template <class ExtDate> inline
    std::shared_ptr<typename DayCounter<ExtDate>::Impl>
    Actual365Fixed<ExtDate>::implementation(Actual365Fixed<ExtDate>::Convention c) {
        switch (c) {
          case Standard:
            return std::shared_ptr<typename DayCounter<ExtDate>::Impl>(new Impl);
          case Canadian:
              return std::shared_ptr<typename DayCounter<ExtDate>::Impl>(new CA_Impl);
          case NoLeap:
              return std::shared_ptr<typename DayCounter<ExtDate>::Impl>(new NL_Impl);
          default:
            QL_FAIL("unknown Actual/365 (Fixed) convention");
        }
    }
    template <class ExtDate> inline
    Time Actual365Fixed<ExtDate>::CA_Impl::yearFraction(const ExtDate& d1,
                                               const ExtDate& d2,
                                               const ExtDate& refPeriodStart,
                                               const ExtDate& refPeriodEnd) const {
        if (to_DateLike(d1) == d2)
            return 0.0;

        // We need the period to calculate the frequency
        QL_REQUIRE(to_DateLike(refPeriodStart) != ExtDate(), "invalid refPeriodStart");
        QL_REQUIRE(to_DateLike(refPeriodEnd) != ExtDate(), "invalid refPeriodEnd");

        Time dcs = daysBetween(to_DateLike(d1),to_DateLike(d2));
        Time dcc = daysBetween(to_DateLike(refPeriodStart),to_DateLike(refPeriodEnd));
        Integer months = Integer(std::lround(12*dcc/365));
        QL_REQUIRE(months != 0,
                   "invalid reference period for Act/365 Canadian; "
                   "must be longer than a month");
        Integer frequency = Integer(12/months);

        if (dcs < Integer(365/frequency))
            return dcs/365.0;

        return 1./frequency - (dcc-dcs)/365.0;

    }
    template <class ExtDate> inline
    serial_type Actual365Fixed<ExtDate>::NL_Impl::dayCount(const ExtDate& dd1,
                                                        const ExtDate& dd2) const {
        auto d1 = to_DateLike(dd1);
        auto d2 = to_DateLike(dd2);
        static const Integer MonthOffset[] = {
            0,  31,  59,  90, 120, 151,  // Jan - Jun
            181, 212, 243, 273, 304, 334   // Jun - Dec
        };

        serial_type s1 = d1.dayOfMonth()
                             + MonthOffset[d1.month()-1] + (d1.year() * 365);
        serial_type s2 = d2.dayOfMonth()
                             + MonthOffset[d2.month()-1] + (d2.year() * 365);

        if (d1.month() == Feb && d1.dayOfMonth() == 29) {
            --s1;
        }

        if (d2.month() == Feb && d2.dayOfMonth() == 29) {
            --s2;
        }

        return s2 - s1;
    }
    template <class ExtDate> inline
    Time Actual365Fixed<ExtDate>::NL_Impl::yearFraction(const ExtDate& d1,
                                               const ExtDate& d2,
                                               const ExtDate& d3,
                                               const ExtDate& d4) const {
        return dayCount(d1, d2)/365.0;
    }

}

