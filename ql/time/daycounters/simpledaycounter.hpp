/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003 RiskMap srl

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

/*! \file simpledaycounter.hpp
     \brief Simple day counter for reproducing theoretical calculations
*/

#ifndef quantlib_simple_day_counter_hpp
#define quantlib_simple_day_counter_hpp

#include "daycounter.hpp"
#include "thirty360.hpp"

namespace QuantLib {

    //! Simple day counter for reproducing theoretical calculations.
    /*! This day counter tries to ensure that whole-month distances
        are returned as a simple fraction, i.e., 1 year = 1.0,
        6 months = 0.5, 3 months = 0.25 and so forth.

        \warning this day counter should be used together with
                 NullCalendar, which ensures that dates at whole-month
                 distances share the same day of month. It is <b>not</b>
                 guaranteed to work with any other calendar.

        \ingroup daycounters

        \test the correctness of the results is checked against known
              good values.
    */
    template <class Date>
    class SimpleDayCounter : public DayCounter<Date> {
      private:
        class Impl : public DayCounter<Date>::Impl {
          public:
            std::string name() const { return "Simple"; }
            typename date_traits<Date>::serial_type dayCount(const Date& d1,
                                       const Date& d2) const;
            typename date_traits<Date>::Time
            yearFraction(const Date& d1,
                              const Date& d2,
                              const Date&,
                              const Date&) const;
        };
      public:
        SimpleDayCounter()
        : DayCounter<Date>(std::shared_ptr<typename DayCounter<Date>::Impl>(
                                             new SimpleDayCounter::Impl())) {}
    };

        namespace {
        template <class Date>
            static DayCounter<Date> fallback = Thirty360<Date>();
    }

    template <class Date>
    inline typename date_traits<Date>::serial_type
    SimpleDayCounter<Date>::Impl::dayCount(const Date& d1, const Date& d2) const {
        return fallback<Date>.dayCount(d1, d2);
    }

    template <class Date>
    inline typename date_traits<Date>::Time SimpleDayCounter<Date>::Impl::yearFraction(
        const Date& d1,
                                              const Date& d2,
                                              const Date&,
                                              const Date&) const {
        auto dm1 = date_traits<Date>::dayOfMonth(d1), dm2 = date_traits<Date>::dayOfMonth(d2);

        if (dm1 == dm2 ||
            // e.g., Aug 30 -> Feb 28 ?
            (dm1 > dm2 && date_traits<Date>::isEndOfMonth(d2)) ||
            // e.g., Feb 28 -> Aug 30 ?
            (dm1 < dm2 && date_traits<Date>::isEndOfMonth(d1))) {

            return (d2.year() - d1.year()) + (int(d2.month()) - int(d1.month())) / 12.0;

        } else {
            return fallback<Date>.yearFraction(d1, d2);
        }
    }

}

#endif
