/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2015 StatPro Italia srl

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

/*! \file thirty365.hpp
    \brief 30/365 day counters
*/

#ifndef quantlib_thirty_365_day_counter_hpp
#define quantlib_thirty_365_day_counter_hpp

#include "daycounter.hpp"

namespace QuantLib {

    //! 30/365 day count convention
    /*! \ingroup daycounters */
    template <class Date>
    class Thirty365 : public DayCounter<Date> {
      private:
        class Impl : public DayCounter<Date>::Impl {
          public:
            std::string name() const { return std::string("30/365"); }
            typename date_traits<Date>::serial_type dayCount(const Date& d1,
                                       const Date& d2) const;
            typename date_traits<Date>::Time
            yearFraction(const Date& d1,
                              const Date& d2,
                              const Date&, 
                              const Date&) const {
                return dayCount(d1,d2)/365.0; }
        };
      public:
        Thirty365();
    };

        template <class Date>
    inline typename date_traits<Date>::serial_type
    Thirty365<Date>::Impl::dayCount(const Date& d1, const Date& d2) const {
        auto dd1 = date_traits<Date>::dayOfMonth(d1), dd2 = date_traits<Date>::dayOfMonth(d2);
        int mm1 = d1.month(), mm2 = d2.month();
        auto yy1 = d1.year(), yy2 = d2.year();

        return 360 * (yy2 - yy1) + 30 * (mm2 - mm1) + (dd2 - dd1);
    }

    template <class Date>
    inline Thirty365<Date>::Thirty365()
    : DayCounter<Date>(std::shared_ptr<typename DayCounter<Date>::Impl>(new Thirty365::Impl)) {}

}

#endif
