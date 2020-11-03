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

/*! \file actualactual.hpp
    \brief act/act day counters
*/

#ifndef quantlib_actualactual_day_counter_h
#define quantlib_actualactual_day_counter_h

#include "daycounter.hpp"
#include "../schedule.hpp"

namespace QuantLib {

    //! Actual/Actual day count
    /*! The day count can be calculated according to:

        - the ISDA convention, also known as "Actual/Actual (Historical)",
          "Actual/Actual", "Act/Act", and according to ISDA also "Actual/365",
          "Act/365", and "A/365";
        - the ISMA and US Treasury convention, also known as
          "Actual/Actual (Bond)";
        - the AFB convention, also known as "Actual/Actual (Euro)".

        For more details, refer to
        https://www.isda.org/a/pIJEE/The-Actual-Actual-Day-Count-Fraction-1999.pdf

        \ingroup daycounters

        \test the correctness of the results is checked against known
              good values.
    */
    template <class Date>
    class ActualActual : public DayCounter<Date> {
      public:
        enum Convention { ISMA, Bond,
                          ISDA, Historical, Actual365,
                          AFB, Euro };
      private:
        class ISMA_Impl : public DayCounter::Impl {
          public:
            explicit ISMA_Impl(const Schedule<Date>& schedule)
            : schedule_(schedule) {}

            std::string name() const {
                return std::string("Actual/Actual (ISMA)");
            }
            typename date_traits<Date>::Time yearFraction(const Date& d1,
                              const Date& d2,
                              const Date& refPeriodStart,
                              const Date& refPeriodEnd) const;
          private:
            Schedule<Date> schedule_;
        };
        class Old_ISMA_Impl : public DayCounter::Impl {
          public:
            std::string name() const {
                return std::string("Actual/Actual (ISMA)");
            }
            typename date_traits<Date>::Time yearFraction(const Date& d1,
                              const Date& d2,
                              const Date& refPeriodStart,
                              const Date& refPeriodEnd) const;
        };
        class ISDA_Impl : public DayCounter::Impl {
          public:
            std::string name() const {
                return std::string("Actual/Actual (ISDA)");
            }
            typename date_traits<Date>::Time
            yearFraction(const Date& d1,
                              const Date& d2,
                              const Date&,
                              const Date&) const;
        };
        class AFB_Impl : public DayCounter::Impl {
          public:
            std::string name() const {
                return std::string("Actual/Actual (AFB)");
            }
            typename date_traits<Date>::Time
            yearFraction(const Date& d1,
                              const Date& d2,
                              const Date&,
                              const Date&) const;
        };
        static std::shared_ptr<DayCounter::Impl> implementation(
                                                               Convention c, 
                                                               const Schedule<Date>& schedule);
      public:
        ActualActual(Convention c = ActualActual::ISDA, 
                     const Schedule<Date>& schedule = Schedule<Date>())
        : DayCounter(implementation(c, schedule)) {}
    };

        namespace {

        // the template argument works around passing a protected type

        template <class T, class Date> inline
        int findCouponsPerYear(const T& impl, Date refStart, Date refEnd) {
            // This will only work for day counts longer than 15 days.
            int months = int(0.5 + 12 * Real(impl.dayCount(refStart, refEnd)) / 365.0);
            return (int)(0.5 + 12.0 / Real(months));
        }

        /* An ISMA day counter either needs a schedule or to have
           been explicitly passed a reference period. This usage
           leads to inaccurate year fractions.
        */
        template <class T, class Date>
        inline typename date_traits<Date>::Time
        yearFractionGuess(const T& impl, const Date& start, const Date& end) {
            // asymptotically correct.
            return Real(impl.dayCount(start, end)) / 365.0;
        }

        template <class Date> inline
            std::vector<Date>
        getListOfPeriodDatesIncludingQuasiPayments(const Schedule<Date>& schedule) {
            // Process the schedule into an array of dates.
            Date issueDate = schedule.date(0);
            Date firstCoupon = schedule.date(1);
            Date notionalCoupon = schedule.calendar().advance(firstCoupon, -schedule.tenor(),
                                                              schedule.businessDayConvention(),
                                                              schedule.endOfMonth());

            std::vector<Date> newDates = schedule.dates();
            newDates[0] = notionalCoupon;

            // long first coupon
            if (notionalCoupon > issueDate) {
                Date priorNotionalCoupon = schedule.calendar().advance(
                    notionalCoupon, -schedule.tenor(), schedule.businessDayConvention(),
                    schedule.endOfMonth());
                newDates.insert(newDates.begin(),
                                priorNotionalCoupon); // insert as the first element?
            }
            return newDates;
        }

        template <class T, class Date>
        typename date_traits<Date>::Time yearFractionWithReferenceDates(
            const T& impl, const Date& d1, const Date& d2, const Date& d3, const Date& d4) {
            QL_REQUIRE(d1 <= d2, "This function is only correct if d1 <= d2\n"
                                 "d1: {} d2: {}" , d1 , d2);

            Real referenceDayCount = Real(impl.dayCount(d3, d4));
            // guess how many coupon periods per year:
            int couponsPerYear;
            if (referenceDayCount < 16) {
                couponsPerYear = 1;
                referenceDayCount = impl.dayCount(d1, date_traits<Date>::plusPeriod(d1 , 1 * Years));
            } else {
                couponsPerYear = findCouponsPerYear(impl, d3, d4);
            }
            return Real(impl.dayCount(d1, d2)) / (referenceDayCount * couponsPerYear);
        }

    }

            template <class Date>
    inline std::shared_ptr<typename DayCounter<Date>::Impl>
    ActualActual<Date>::implementation(ActualActual::Convention c, const Schedule<Date>& schedule) {
        switch (c) {
            case ISMA:
            case Bond:
                if (!schedule.empty())
                    return std::shared_ptr<DayCounter::Impl>(new ISMA_Impl(schedule));
                else
                    return std::shared_ptr<DayCounter::Impl>(new Old_ISMA_Impl);
            case ISDA:
            case Historical:
            case Actual365:
                return std::shared_ptr<DayCounter::Impl>(new ISDA_Impl);
            case AFB:
            case Euro:
                return std::shared_ptr<DayCounter::Impl>(new AFB_Impl);
            default:
                QL_FAIL("unknown act/act convention");
        }
    }


    using Integer = int;
    template <class Date> inline
    typename date_traits<Date>::Time ActualActual<Date>::ISMA_Impl::yearFraction(
        const Date& d1,
                                               const Date& d2,
                                               const Date& d3,
                                               const Date& d4) const {
        if (d1 == d2) {
            return 0.0;
        } else if (d2 < d1) {
            return -yearFraction(d2, d1, d3, d4);
        }

        std::vector<Date> couponDates = getListOfPeriodDatesIncludingQuasiPayments(schedule_);

        Real yearFractionSum = 0.0;
        for (Size i = 0; i < couponDates.size() - 1; i++) {
            Date startReferencePeriod = couponDates[i];
            Date endReferencePeriod = couponDates[i + 1];
            if (d1 < endReferencePeriod && d2 > startReferencePeriod) {
                yearFractionSum += yearFractionWithReferenceDates(
                    *this, std::max(d1, startReferencePeriod), std::min(d2, endReferencePeriod),
                    startReferencePeriod, endReferencePeriod);
            }
        }
        return yearFractionSum;
    }


            template <class Date>
    inline typename date_traits<Date>::Time ActualActual<Date>::Old_ISMA_Impl::yearFraction(
        const Date& d1,
                                                   const Date& d2,
                                                   const Date& d3,
                                                   const Date& d4) const {
        if (d1 == d2)
            return 0.0;

        if (d1 > d2)
            return -yearFraction(d2, d1, d3, d4);

        // when the reference period is not specified, try taking
        // it equal to (d1,d2)
        Date refPeriodStart = (d3 != Date() ? d3 : d1);
        Date refPeriodEnd = (d4 != Date() ? d4 : d2);

        QL_REQUIRE(refPeriodEnd > refPeriodStart && refPeriodEnd > d1,
                   "invalid reference period: date 1:  {}, date 2: {}, reference period start: {}, reference period end: {}" , d1 , d2 , refPeriodStart 
                       , refPeriodEnd);

        // estimate roughly the length in months of a period
        int months =
            int(0.5 +
                12 * Real(date_traits<Date>::onlyDaysBetween(refPeriodStart, refPeriodEnd )) / 365);

        // for short periods...
        if (months == 0) {
            // ...take the reference period as 1 year from d1
            refPeriodStart = d1;
            refPeriodEnd = date_traits<Date>::plusPeriod(d1, 1 * Years);
            months = 12;
        }

         date_traits<Date>::Time period = Real(months) / 12.0;

        if (d2 <= refPeriodEnd) {
            // here refPeriodEnd is a future (notional?) payment date
            if (d1 >= refPeriodStart) {
                // here refPeriodStart is the last (maybe notional)
                // payment date.
                // refPeriodStart <= d1 <= d2 <= refPeriodEnd
                // [maybe the equality should be enforced, since
                // refPeriodStart < d1 <= d2 < refPeriodEnd
                // could give wrong results] ???
                return period * Real(date_traits<Date>::daysBetween(d1, d2)) /
                       date_traits<Date>::daysBetween(refPeriodStart, refPeriodEnd);
            } else {
                // here refPeriodStart is the next (maybe notional)
                // payment date and refPeriodEnd is the second next
                // (maybe notional) payment date.
                // d1 < refPeriodStart < refPeriodEnd
                // AND d2 <= refPeriodEnd
                // this case is long first coupon

                // the last notional payment date
                Date previousRef = date_traits<Date>::plusPeriod( refPeriodStart , - months * Months);

                if (d2 > refPeriodStart)
                    return yearFraction(d1, refPeriodStart, previousRef, refPeriodStart) +
                           yearFraction(refPeriodStart, d2, refPeriodStart, refPeriodEnd);
                else
                    return yearFraction(d1, d2, previousRef, refPeriodStart);
            }
        } else {
            // here refPeriodEnd is the last (notional?) payment date
            // d1 < refPeriodEnd < d2 AND refPeriodStart < refPeriodEnd
            QL_REQUIRE(refPeriodStart <= d1, "invalid dates: "
                                             "d1 < refPeriodStart < refPeriodEnd < d2");
            // now it is: refPeriodStart <= d1 < refPeriodEnd < d2

            // the part from d1 to refPeriodEnd
            date_traits<Date>::Time sum =
                yearFraction(d1, refPeriodEnd, refPeriodStart, refPeriodEnd);

            // the part from refPeriodEnd to d2
            // count how many regular periods are in [refPeriodEnd, d2],
            // then add the remaining time
            int i = 0;
            Date newRefStart, newRefEnd;
            for (;;) {
                newRefStart = date_traits<Date>::plusPeriod(refPeriodEnd , (months * i) * Months);
                newRefEnd = date_traits<Date>::plusPeriod(refPeriodEnd , (months * (i + 1)) * Months);
                if (d2 < newRefEnd) {
                    break;
                } else {
                    sum += period;
                    i++;
                }
            }
            sum += yearFraction(newRefStart, d2, newRefStart, newRefEnd);
            return sum;
        }
    }


            template <class Date>
    inline typename date_traits<Date>::Time ActualActual<Date>::ISDA_Impl::yearFraction(
        const Date& d1,
                                               const Date& d2,
                                               const Date&,
                                               const Date&) const {
        if (d1 == d2)
            return 0.0;

        if (d1 > d2)
            return -yearFraction(d2, d1, Date(), Date());

        int y1 = d1.year(), y2 = d2.year();
        Real dib1 = (date_traits<Date>::isLeap(y1) ? 366.0 : 365.0),
             dib2 = (date_traits<Date>::isLeap(y2) ? 366.0 : 365.0);

         date_traits<Date>::Time sum = y2 - y1 - 1;
        // FLOATING_POINT_EXCEPTION
         sum += date_traits<Date>::daysBetween(d1, Date(1, January, y1 + 1)) / dib1;
         sum += date_traits<Date>::daysBetween(Date(1, January, y2), d2) / dib2;
        return sum;
    }


            template <class Date>
    inline typename date_traits<Date>::Time ActualActual<Date>::AFB_Impl::yearFraction(
        const Date& d1,
                                              const Date& d2,
                                              const Date&,
                                              const Date&) const {
        if (d1 == d2)
            return 0.0;

        if (d1 > d2)
            return -yearFraction(d2, d1, Date(), Date());

        Date newD2 = d2, temp = d2;
        date_traits<Date>::Time sum = 0.0;
        while (temp > d1) {
            temp = date_traits<Date>::plusPeriod( newD2 , - 1 * Years );
            if (date_traits<Date>::dayOfMonth(temp) == 28 && temp.month() == 2 &&
               date_traits<Date>::isLeap(temp.year())) {
                date_traits<Date>::inc(temp);
            }
            if (temp >= d1) {
                sum += 1.0;
                newD2 = temp;
            }
        }

        Real den = 365.0;

        if (date_traits<Date>::isLeap(newD2.year())) {
            temp = Date(29, February, newD2.year());
            if (newD2 > temp && d1 <= temp)
                den += 1.0;
        } else if (date_traits<Date>::isLeap(d1.year())) {
            temp = Date(29, February, d1.year());
            if (newD2 > temp && d1 <= temp)
                den += 1.0;
        }

        return sum + date_traits<Date>::daysBetween(d1, newD2) / den;
    }


}

#endif
