/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2004 Ferdinando Ametrano
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

/*! \file actual365fixed.hpp
    \brief Actual/365 (Fixed) day counter
*/
#pragma once
#ifndef quantlib_actual365fixed_day_counter_h
#define quantlib_actual365fixed_day_counter_h

#include <ql/time/daycounter.hpp>

namespace QuantLib {

    //! Actual/365 (Fixed) day count convention
    /*! "Actual/365 (Fixed)" day count convention, also know as
        "Act/365 (Fixed)", "A/365 (Fixed)", or "A/365F".

        \warning According to ISDA, "Actual/365" (without "Fixed") is
                 an alias for "Actual/Actual (ISDA)" (see
                 ActualActual.)  If Actual/365 is not explicitly
                 specified as fixed in an instrument specification,
                 you might want to double-check its meaning.

        \ingroup daycounters
    */template <class ExtDate=Date>
    class Actual365Fixed : public DayCounter<ExtDate> {
      public:
        enum Convention { Standard, Canadian, NoLeap };
        explicit Actual365Fixed(Convention c = Actual365Fixed::Standard)
        : DayCounter<ExtDate>(implementation(c)) {}

      private:
        class Impl : public DayCounter<ExtDate>::Impl {
          public:
            std::string name() const { return std::string("Actual/365 (Fixed)"); }
            Time yearFraction(const ExtDate& d1,
                              const ExtDate& d2,
                              const ExtDate&,
                              const ExtDate&) const {
                return daysBetween(to_DateLike(d1),to_DateLike(d2))/365.0;
            }
        };
        class CA_Impl : public DayCounter<ExtDate>::Impl {
          public:
            std::string name() const {
                return std::string("Actual/365 (Fixed) Canadian Bond");
            }
            Time yearFraction(const ExtDate& d1,
                              const ExtDate& d2,
                              const ExtDate& refPeriodStart,
                              const ExtDate& refPeriodEnd) const;
        };
        class NL_Impl : public DayCounter<ExtDate>::Impl {
          public:
            std::string name() const {
                return std::string("Actual/365 (No Leap)");
            }
            serial_type dayCount(const ExtDate& d1,
                                       const ExtDate& d2) const;
            Time yearFraction(const ExtDate& d1,
                              const ExtDate& d2,
                              const ExtDate& refPeriodStart,
                              const ExtDate& refPeriodEnd) const;
        };
        static std::shared_ptr<typename DayCounter<ExtDate>::Impl> implementation(Convention);
    };

}
#include "actual365fixed.cpp"
#endif
