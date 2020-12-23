/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2018 Alexey Indiryakov

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

#include <ql/time/daycounters/thirty360.hpp>
#include <algorithm>

namespace QuantLib {
    template <class ExtDate> inline std::shared_ptr<typename DayCounter<ExtDate>::Impl>
    Thirty360<ExtDate>::implementation(Thirty360<ExtDate>::Convention c, bool isLastPeriod) {
        switch (c) {
          case USA:
          case BondBasis:
              return std::shared_ptr<typename DayCounter<ExtDate>::Impl>(new US_Impl);
          case European:
          case EurobondBasis:
              return std::shared_ptr<typename DayCounter<ExtDate>::Impl>(new EU_Impl);
          case Italian:
              return std::shared_ptr<typename DayCounter<ExtDate>::Impl>(new IT_Impl);
          case German:
              return std::shared_ptr<typename DayCounter<ExtDate>::Impl>(
                new GER_Impl(isLastPeriod));
          default:
            QL_FAIL("unknown 30/360 convention");
        }
    }
    template <class ExtDate> inline
    serial_type Thirty360<ExtDate>::US_Impl::dayCount(const ExtDate& ed1,
                                                   const ExtDate& ed2) const {
        auto d1 = to_DateLike(ed1);
        auto d2 = to_DateLike(ed2);
        Day dd1 = d1.dayOfMonth(), dd2 = d2.dayOfMonth();
        Integer mm1 = d1.month(), mm2 = d2.month();
        Year yy1 = d1.year(), yy2 = d2.year();

        if (dd2 == 31 && dd1 < 30) { dd2 = 1; mm2++; }

        return 360*(yy2-yy1) + 30*(mm2-mm1-1) +
            std::max(Integer(0),30-dd1) + std::min(Integer(30),dd2);
    }
    template <class ExtDate> inline
    serial_type Thirty360<ExtDate>::EU_Impl::dayCount(const ExtDate& ed1,
                                                   const ExtDate& ed2) const {
        auto d1 = to_DateLike(ed1);
        auto d2 = to_DateLike(ed2);
        Day dd1 = d1.dayOfMonth(), dd2 = d2.dayOfMonth();
        Month mm1 = d1.month(), mm2 = d2.month();
        Year yy1 = d1.year(), yy2 = d2.year();

        return 360*(yy2-yy1) + 30*(mm2-mm1-1) +
            std::max(Integer(0),30-dd1) + std::min(Integer(30),dd2);
    }
    template <class ExtDate> inline
    serial_type Thirty360<ExtDate>::IT_Impl::dayCount(const ExtDate& ed1,
                                                   const ExtDate& ed2) const {
        auto d1 = to_DateLike(ed1);
        auto d2 = to_DateLike(ed2);
        Day dd1 = d1.dayOfMonth(), dd2 = d2.dayOfMonth();
        Month mm1 = d1.month(), mm2 = d2.month();
        Year yy1 = d1.year(), yy2 = d2.year();

        if (mm1 == 2 && dd1 > 27) dd1 = 30;
        if (mm2 == 2 && dd2 > 27) dd2 = 30;

        return 360*(yy2-yy1) + 30*(mm2-mm1-1) +
            std::max(Integer(0),30-dd1) + std::min(Integer(30),dd2);
    }
    template <class ExtDate> inline
    serial_type Thirty360<ExtDate>::GER_Impl::dayCount(const ExtDate& ed1,
                                                    const ExtDate& ed2) const {
        auto d1 = to_DateLike(ed1);
        auto d2 = to_DateLike(ed2);
        Day dd1 = d1.dayOfMonth(), dd2 = d2.dayOfMonth();
        Month mm1 = d1.month(), mm2 = d2.month();
        Year yy1 = d1.year(), yy2 = d2.year();

        if (mm1 == 2 && dd1 == 28 + (DateLike<ExtDate>::isLeap(yy1) ? 1 : 0))
            dd1 = 30;
        if (!isLastPeriod_ && mm2 == 2 && dd2 == 28 + (DateLike<ExtDate>::isLeap(yy2) ? 1 : 0))
            dd2 = 30;

        return 360*(yy2-yy1) + 30*(mm2-mm1-1) +
            std::max(Integer(0),30-dd1) + std::min(Integer(30),dd2);
    }

}
