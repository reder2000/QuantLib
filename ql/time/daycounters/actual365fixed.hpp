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

#ifndef quantlib_actual365fixed_day_counter_h
#define quantlib_actual365fixed_day_counter_h

#include "daycounter.hpp"

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
    */
    template <class Date>
    class Actual365Fixed : public DayCounter<Date> {
      public:
        enum Convention { Standard, Canadian, NoLeap };
        explicit Actual365Fixed(Convention c = Actual365Fixed::Standard)
        : DayCounter<Date> (implementation(c)) {}

      private:
        class Impl : public DayCounter<Date>::Impl {
          public:
            std::string name() const { return std::string("Actual/365 (Fixed)"); }
            typename type_traits<Date>::Time
            yearFraction(const Date& d1,
                              const Date& d2,
                              const Date&,
                              const Date&) const {
                return type_traits<Date>::daysBetween(d1, d2) / 365.0;
            }
        };
        class CA_Impl : public DayCounter<Date>::Impl {
          public:
            std::string name() const {
                return std::string("Actual/365 (Fixed) Canadian Bond");
            }
            typename type_traits<Date>::Time yearFraction(const Date& d1,
                              const Date& d2,
                              const Date& refPeriodStart,
                              const Date& refPeriodEnd) const;
        };
        class NL_Impl : public DayCounter<Date>::Impl {
          public:
            std::string name() const {
                return std::string("Actual/365 (No Leap)");
            }
            typename type_traits<Date>::serial_type dayCount(const Date& d1,
                                       const Date& d2) const;
            typename type_traits<Date>::Time yearFraction(const Date& d1,
                              const Date& d2,
                              const Date& refPeriodStart,
                              const Date& refPeriodEnd) const;
        };
        static std::shared_ptr<typename DayCounter<Date>::Impl> implementation(Convention);
    };

    template <class Date>
    inline std::shared_ptr<typename DayCounter<Date>::Impl>
    Actual365Fixed<Date>::implementation(Actual365Fixed::Convention c) {
        switch (c) {
            case Standard:
                return std::shared_ptr<typename DayCounter<Date>::Impl>(new Impl);
            case Canadian:
                return std::shared_ptr<typename DayCounter<Date>::Impl>(new CA_Impl);
            case NoLeap:
                return std::shared_ptr<typename DayCounter<Date>::Impl>(new NL_Impl);
            default:
                QL_FAIL("unknown Actual/365 (Fixed) convention");
        }
    }

template <class Date>
    inline typename type_traits<Date>::Time
    Actual365Fixed<Date>::CA_Impl::yearFraction(const Date& d1,
                                               const Date& d2,
                                               const Date& refPeriodStart,
                                               const Date& refPeriodEnd) const {
        if (d1 == d2)
            return 0.0;

        // We need the period to calculate the frequency
        QL_REQUIRE(refPeriodStart != Date(), "invalid refPeriodStart");
        QL_REQUIRE(refPeriodEnd != Date(), "invalid refPeriodEnd");

        typename type_traits<Date>::Time dcs =  type_traits<Date>::daysBetween(d1, d2);
        typename type_traits<Date>::Time dcc =
            type_traits<Date>::daysBetween(refPeriodStart, refPeriodEnd);
        int months = int(0.5 + 12 * dcc / 365);
        QL_REQUIRE(months != 0, "invalid reference period for Act/365 Canadian; "
                                "must be longer than a month");
        int frequency = int(12 / months);

        if (dcs < int(365 / frequency))
            return dcs / 365.0;

        return 1. / frequency - (dcc - dcs) / 365.0;
    }

    template <class Date>
    inline typename type_traits<Date>::serial_type
    Actual365Fixed<Date>::NL_Impl::dayCount(const Date& d1, const Date& d2) const {

        static const int MonthOffset[] = {
            0,   31,  59,  90,  120, 151, // Jan - Jun
            181, 212, 243, 273, 304, 334  // Jun - Dec
        };

        typename type_traits<Date>::serial_type s1 = dayOfMonth(d1) + MonthOffset[d1.month() - 1] + (d1.year() * 365);
        typename type_traits<Date>::serial_type s2 =
            dayOfMonth(d2) + MonthOffset[d2.month() - 1] + (d2.year() * 365);

        if (d1.month() == Feb && dayOfMonth(d1) == 29) {
            --s1;
        }

        if (d2.month() == Feb && dayOfMonth(d2) == 29) {
            --s2;
        }

        return s2 - s1;
    }

        template <class Date>
        inline typename type_traits<Date>::Time Actual365Fixed<Date>::NL_Impl::yearFraction(
        const Date& d1,
                                               const Date& d2,
                                               const Date& d3,
                                               const Date& d4) const {
        return dayCount(d1, d2) / 365.0;
    }


}

#endif
