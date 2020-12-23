/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Piter Dias
 Copyright (C) 2011 StatPro Italia srl

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

/*! \file business252.hpp
    \brief business/252 day counter
*/
#pragma once
#ifndef quantlib_business252_day_counter_hpp
#define quantlib_business252_day_counter_hpp

#include <ql/time/calendar.hpp>
#include <ql/time/calendars/brazil.hpp>
#include <ql/time/daycounter.hpp>
#include <utility>
#pragma once
namespace QuantLib {

    //! Business/252 day count convention
    /*! \ingroup daycounters */
    template <class ExtDate = Date>
    class Business252 : public DayCounter<ExtDate> {
      private:
        class Impl : public DayCounter<ExtDate>::Impl {
          private:
            Calendar<ExtDate> calendar_;
          public:
            std::string name() const;
            serial_type dayCount(const ExtDate& d1,
                                       const ExtDate& d2) const;
            Time yearFraction(const ExtDate& d1,
                              const ExtDate& d2,
                              const ExtDate&,
                              const ExtDate&) const;
            explicit Impl(const Calendar<ExtDate>& c) : calendar_(c) {}
        };
      public:
        Business252(const Calendar<ExtDate>& c = Brazil<ExtDate>())
        : DayCounter<ExtDate>(
              std::shared_ptr<typename DayCounter<ExtDate>::Impl>(new Business252::Impl(c))) {}
    };

}
#include "business252.cpp"
#endif
