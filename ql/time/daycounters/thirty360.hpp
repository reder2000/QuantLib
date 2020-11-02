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

/*! \file thirty360.hpp
    \brief 30/360 day counters
*/

#ifndef quantlib_thirty360_day_counter_h
#define quantlib_thirty360_day_counter_h

#include "daycounter.hpp"

namespace QuantLib {

    //! 30/360 day count convention
    /*! The 30/360 day count can be calculated according to US, European,
        Italian or German conventions.

        US (NASD) convention: if the starting date is the 31st of a
        month, it becomes equal to the 30th of the same month.
        If the ending date is the 31st of a month and the starting
        date is earlier than the 30th of a month, the ending date
        becomes equal to the 1st of the next month, otherwise the
        ending date becomes equal to the 30th of the same month.
        Also known as "30/360", "360/360", or "Bond Basis"

        European convention: starting dates or ending dates that
        occur on the 31st of a month become equal to the 30th of the
        same month.
        Also known as "30E/360", or "Eurobond Basis"

        Italian convention: starting dates or ending dates that
        occur on February and are grater than 27 become equal to 30
        for computational sake.

        German convention: starting dates or ending dates that
        occur on the last day of February become equal to 30
        for computational sake, except for the termination date.
        Also known as "30E/360 ISDA"

        \ingroup daycounters
    */
    template <class Date>
    class Thirty360 : public DayCounter<Date> {
      public:
        enum Convention { USA, BondBasis,
                          European, EurobondBasis,
                          Italian,
                          German };
      private:
        class US_Impl : public DayCounter<Date>::Impl {
          public:
            std::string name() const { return std::string("30/360 (Bond Basis)");}
            typename type_traits<Date>::serial_type dayCount(const Date& d1,
                                       const Date& d2) const;
            typename type_traits<Date>::Time
            yearFraction(const Date& d1,
                              const Date& d2,
                              const Date&,
                              const Date&) const {
                return dayCount(d1,d2)/360.0; }
        };
        class EU_Impl : public DayCounter<Date>::Impl {
          public:
            std::string name() const { return std::string("30E/360 (Eurobond Basis)");}
            typename type_traits<Date>::serial_type dayCount(const Date& d1,
                                       const Date& d2) const;
            typename type_traits<Date>::Time
            yearFraction(const Date& d1,
                              const Date& d2,
                              const Date&,
                              const Date&) const {
                return dayCount(d1,d2)/360.0; }
        };
        class IT_Impl : public DayCounter<Date>::Impl {
          public:
            std::string name() const { return std::string("30/360 (Italian)");}
            typename type_traits<Date>::serial_type dayCount(const Date& d1, const Date& d2) const;
            typename type_traits<Date>::Time
            yearFraction(const Date& d1,
                              const Date& d2,
                              const Date&,
                              const Date&) const {
                return dayCount(d1,d2)/360.0; }
        };
        class GER_Impl : public DayCounter<Date>::Impl {
          public:
            explicit GER_Impl(bool isLastPeriod) : isLastPeriod_(isLastPeriod) {}
            std::string name() const { return std::string("30/360 (German)");}
            typename type_traits<Date>::serial_type dayCount(const Date& d1, const Date& d2) const;
            typename type_traits<Date>::Time
            yearFraction(const Date& d1,
                              const Date& d2,
                              const Date&,
                              const Date&) const {
                return dayCount(d1,d2)/360.0; }
        private:
            bool isLastPeriod_;
        };
        static std::shared_ptr<typename DayCounter<Date>::Impl> implementation(
            Convention c, bool isLastPeriod);
      public:
        Thirty360(Convention c = Thirty360::BondBasis, bool isLastPeriod = false)
        : DayCounter<Date>(implementation(c, isLastPeriod)) {}
    };

        template <class Date>
    inline std::shared_ptr<typename DayCounter<Date>::Impl>
    Thirty360<Date>::implementation(Thirty360::Convention c, bool isLastPeriod) {
        switch (c) {
            case USA:
            case BondBasis:
                return std::shared_ptr<typename DayCounter<Date>::Impl>(new US_Impl);
            case European:
            case EurobondBasis:
                return std::shared_ptr<typename DayCounter<Date>::Impl>(new EU_Impl);
            case Italian:
                return std::shared_ptr<typename DayCounter<Date>::Impl>(new IT_Impl);
            case German:
                return std::shared_ptr<typename DayCounter<Date>::Impl>(new GER_Impl(isLastPeriod));
            default:
                QL_FAIL("unknown 30/360 convention");
        }
    }

    template <class Date>
    inline typename type_traits<Date>::serial_type
    Thirty360<Date>::US_Impl::dayCount(const Date& d1, const Date& d2) const {
        auto dd1 = dayOfMonth(d1), dd2 = dayOfMonth(d2);
        int mm1 = d1.month(), mm2 = d2.month();
        auto yy1 = d1.year(), yy2 = d2.year();

        if (dd2 == 31 && dd1 < 30) {
            dd2 = 1;
            mm2++;
        }

        return 360 * (yy2 - yy1) + 30 * (mm2 - mm1 - 1) + std::max(int(0), 30 - dd1) +
               std::min(int(30), dd2);
    }

    template <class Date>
    inline typename type_traits<Date>::serial_type
    Thirty360<Date>::EU_Impl::dayCount(const Date& d1, const Date& d2) const {
        auto dd1 = dayOfMonth(d1), dd2 = dayOfMonth(d2);
        auto mm1 = d1.month(), mm2 = d2.month();
        auto yy1 = d1.year(), yy2 = d2.year();

        return 360 * (yy2 - yy1) + 30 * (mm2 - mm1 - 1) + std::max(int(0), 30 - dd1) +
               std::min(int(30), dd2);
    }

    template <class Date>
    inline typename type_traits<Date>::serial_type
    Thirty360<Date>::IT_Impl::dayCount(const Date& d1, const Date& d2) const {
        auto dd1 = dayOfMonth(d1), dd2 = dayOfMonth(d2);
        auto mm1 = d1.month(), mm2 = d2.month();
        auto yy1 = d1.year(), yy2 = d2.year();

        if (mm1 == 2 && dd1 > 27)
            dd1 = 30;
        if (mm2 == 2 && dd2 > 27)
            dd2 = 30;

        return 360 * (yy2 - yy1) + 30 * (mm2 - mm1 - 1) + std::max(int(0), 30 - dd1) +
               std::min(int(30), dd2);
    }

    template <class Date>
    inline typename type_traits<Date>::serial_type
    Thirty360<Date>::GER_Impl::dayCount(const Date& d1, const Date& d2) const {
        auto dd1 = dayOfMonth(d1), dd2 = dayOfMonth(d2);
        auto mm1 = d1.month(), mm2 = d2.month();
        auto yy1 = d1.year(), yy2 = d2.year();

        if (mm1 == 2 && dd1 == 28 + (type_traits<Date>::isLeap(yy1) ? 1 : 0))
            dd1 = 30;
        if (!isLastPeriod_ && mm2 == 2 && dd2 == 28 + (type_traits<Date>::isLeap(yy2) ? 1 : 0))
            dd2 = 30;

        return 360 * (yy2 - yy1) + 30 * (mm2 - mm1 - 1) + std::max(int(0), 30 - dd1) +
               std::min(int(30), dd2);
    }
}

#endif
