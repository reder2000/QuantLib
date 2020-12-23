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

#include <ql/time/daycounters/actualactual.hpp>
#include <algorithm>
#include <cmath>

namespace QuantLib {

    namespace {

        // the template argument works around passing a protected type

        template <class T,class ExtDate> inline
        Integer findCouponsPerYear(const T& impl,
                                   ExtDate refStart, ExtDate refEnd) {
            // This will only work for day counts longer than 15 days.
            Integer months = (Integer)std::lround(12 * Real(impl.dayCount(refStart, refEnd))/365.0);
            return (Integer)std::lround(12.0 / Real(months));
        }

        /* An ISMA day counter either needs a schedule or to have
           been explicitly passed a reference period. This usage
           leads to inaccurate year fractions.
        */
        template <class T,class ExtDate> inline
        Time yearFractionGuess(const T& impl,
                               const ExtDate& start, const ExtDate& end) {
            // asymptotically correct.
            return Real(impl.dayCount(start, end)) / 365.0;
        }
        template <class ExtDate> inline
        std::vector<ExtDate> getListOfPeriodDatesIncludingQuasiPayments(
                                                   const Schedule<ExtDate>& schedule) {
            // Process the schedule into an array of dates.
            ExtDate issueDate = schedule.date(0);
            ExtDate firstCoupon = schedule.date(1);
            ExtDate notionalCoupon =
                schedule.calendar().advance(firstCoupon,
                                            -schedule.tenor(),
                                            schedule.businessDayConvention(),
                                            schedule.endOfMonth());

            std::vector<ExtDate> newDates = schedule.dates();
            newDates[0] = notionalCoupon;

            //long first coupon
            if (to_DateLike(notionalCoupon) > issueDate) {
                ExtDate priorNotionalCoupon =
                    schedule.calendar().advance(notionalCoupon,
                                                -schedule.tenor(),
                                                schedule.businessDayConvention(),
                                                schedule.endOfMonth());
                newDates.insert(newDates.begin(),
                                priorNotionalCoupon); //insert as the first element?
            }
            return newDates;
        }

        template <class T,class ExtDate> inline
        Time yearFractionWithReferenceDates(const T& impl,
                                            const ExtDate& d1, const ExtDate& d2,
                                            const ExtDate& d3, const ExtDate& d4) {
            QL_REQUIRE(to_DateLike(d1) <= d2,
                       "This function is only correct if d1 <= d2\n"
                       "d1: {} d2: {}"  , d1, d2);

            Real referenceDayCount = Real(impl.dayCount(d3, d4));
            //guess how many coupon periods per year:
            Integer couponsPerYear;
            if (referenceDayCount < 16) {
                couponsPerYear = 1;
                referenceDayCount = impl.dayCount(d1, to_DateLike(d1) + 1 * Years);
            }
            else {
                couponsPerYear = findCouponsPerYear(impl, d3, d4);
            }
            return Real(impl.dayCount(d1, d2)) / (referenceDayCount*couponsPerYear);
        }

    }
    template <class ExtDate> inline
    std::shared_ptr<typename DayCounter<ExtDate>::Impl>
    ActualActual<ExtDate>::implementation(ActualActual<ExtDate>::Convention c,
                                 const Schedule<ExtDate>& schedule) {
        switch (c) {
          case ISMA:
          case Bond:
            if (!schedule.empty())
                  return std::shared_ptr<typename DayCounter<ExtDate>::Impl>(new ISMA_Impl(schedule));
            else
                return std::shared_ptr<typename DayCounter<ExtDate>::Impl>(new Old_ISMA_Impl);
          case ISDA:
          case Historical:
          case Actual365:
              return std::shared_ptr<typename DayCounter<ExtDate>::Impl>(new ISDA_Impl);
          case AFB:
          case Euro:
              return std::shared_ptr<typename DayCounter<ExtDate>::Impl>(new AFB_Impl);
          default:
            QL_FAIL("unknown act/act convention");
        }
    }

    template <class ExtDate> inline
    Time ActualActual<ExtDate>::ISMA_Impl::yearFraction(const ExtDate& d1,
                                               const ExtDate& d2,
                                               const ExtDate& d3,
                                               const ExtDate& d4) const {
        if (to_DateLike(d1) == d2) {
            return 0.0;
        } else if (to_DateLike(d2) < d1) {
            return -yearFraction(d2, d1, d3, d4);
        }

        std::vector<ExtDate> couponDates =
            getListOfPeriodDatesIncludingQuasiPayments<ExtDate>(schedule_);

        Real yearFractionSum = 0.0;
        for (Size i = 0; i < couponDates.size() - 1; i++) {
            ExtDate startReferencePeriod = couponDates[i];
            ExtDate endReferencePeriod = couponDates[i + 1];
            if (to_DateLike(d1) < endReferencePeriod && to_DateLike(d2) > startReferencePeriod) {
                yearFractionSum +=
                    yearFractionWithReferenceDates<ActualActual<ExtDate>::ISMA_Impl,ExtDate>(*this,
                                                   std::max(to_DateLike(d1), to_DateLike(startReferencePeriod)).asExtDate(),
                                                   std::min(to_DateLike(d2), to_DateLike(endReferencePeriod)).asExtDate(),
                                                   startReferencePeriod,
                                                   endReferencePeriod);
            }
        }
        return yearFractionSum;
    }

    template <class ExtDate> inline
    Time ActualActual<ExtDate>::Old_ISMA_Impl::yearFraction(const ExtDate& d1,
                                                   const ExtDate& d2,
                                                   const ExtDate& d3,
                                                   const ExtDate& d4) const {
        if (to_DateLike(d1) == d2)
            return 0.0;

        if (to_DateLike(d1) > d2)
            return -yearFraction(d2,d1,d3,d4);

        // when the reference period is not specified, try taking
        // it equal to (d1,d2)
        ExtDate refPeriodStart = (to_DateLike(d3) != ExtDate() ? d3 : d1);
        ExtDate refPeriodEnd = (to_DateLike(d4) != ExtDate() ? d4 : d2);

        QL_REQUIRE(to_DateLike(refPeriodEnd) > to_DateLike(refPeriodStart) && to_DateLike(refPeriodEnd) > d1,
                   "invalid reference period: date 1: {}, date 2: {}"
                   ", reference period start: {}" 
                   ", reference period end: {}",
                   d1, d2, refPeriodStart , refPeriodEnd);

        // estimate roughly the length in months of a period
        Integer months =
            (Integer)std::lround(12*Real(to_DateLike(refPeriodEnd)-to_DateLike(refPeriodStart))/365);

        // for short periods...
        if (months == 0) {
            // ...take the reference period as 1 year from d1
            refPeriodStart = d1;
            refPeriodEnd = to_DateLike(d1) + 1*Years;
            months = 12;
        }

        Time period = Real(months)/12.0;

        if (to_DateLike(d2) <= refPeriodEnd) {
            // here refPeriodEnd is a future (notional?) payment date
            if (to_DateLike(d1) >= refPeriodStart) {
                // here refPeriodStart is the last (maybe notional)
                // payment date.
                // refPeriodStart <= d1 <= d2 <= refPeriodEnd
                // [maybe the equality should be enforced, since
                // refPeriodStart < d1 <= d2 < refPeriodEnd
                // could give wrong results] ???
                return period*Real(daysBetween(to_DateLike(d1),to_DateLike(d2))) /
                    daysBetween(to_DateLike(refPeriodStart),to_DateLike(refPeriodEnd));
            } else {
                // here refPeriodStart is the next (maybe notional)
                // payment date and refPeriodEnd is the second next
                // (maybe notional) payment date.
                // d1 < refPeriodStart < refPeriodEnd
                // AND d2 <= refPeriodEnd
                // this case is long first coupon

                // the last notional payment date
                ExtDate previousRef = to_DateLike(refPeriodStart) - months*Months;

                if (to_DateLike(d2) > refPeriodStart)
                    return yearFraction(d1, refPeriodStart, previousRef,
                                        refPeriodStart) +
                        yearFraction(refPeriodStart, d2, refPeriodStart,
                                     refPeriodEnd);
                else
                    return yearFraction(d1,d2,previousRef,refPeriodStart);
            }
        } else {
            // here refPeriodEnd is the last (notional?) payment date
            // d1 < refPeriodEnd < d2 AND refPeriodStart < refPeriodEnd
            QL_REQUIRE(to_DateLike(refPeriodStart)<=d1,
                       "invalid dates: "
                       "d1 < refPeriodStart < refPeriodEnd < d2");
            // now it is: refPeriodStart <= d1 < refPeriodEnd < d2

            // the part from d1 to refPeriodEnd
            Time sum = yearFraction(d1, refPeriodEnd,
                                    refPeriodStart, refPeriodEnd);

            // the part from refPeriodEnd to d2
            // count how many regular periods are in [refPeriodEnd, d2],
            // then add the remaining time
            Integer i=0;
            ExtDate newRefStart, newRefEnd;
            for (;;) {
                newRefStart = to_DateLike(refPeriodEnd) + (months*i)*Months;
                newRefEnd = to_DateLike(refPeriodEnd) + (months*(i+1))*Months;
                if (to_DateLike(d2) < newRefEnd) {
                    break;
                } else {
                    sum += period;
                    i++;
                }
            }
            sum += yearFraction(newRefStart,d2,newRefStart,newRefEnd);
            return sum;
        }
    }

    template <class ExtDate> inline
    Time ActualActual<ExtDate>::ISDA_Impl::yearFraction(const ExtDate& dd1,
                                               const ExtDate& dd2,
                                               const ExtDate&,
                                               const ExtDate&) const {
        auto d1 = to_DateLike(dd1);
        auto d2 = to_DateLike(dd2);
        if (d1 == d2)
            return 0.0;

        if (d1 > d2)
            return -yearFraction(d2,d1,ExtDate(),ExtDate());

        Integer y1 = d1.year(), y2 = d2.year();
        Real dib1 = (DateLike<ExtDate>::isLeap(y1) ? 366.0 : 365.0),
             dib2 = (DateLike<ExtDate>::isLeap(y2) ? 366.0 : 365.0);

        Time sum = y2 - y1 - 1;
        // FLOATING_POINT_EXCEPTION
        sum += daysBetween(d1, to_DateLike(DateAdaptor<ExtDate>::Date(1,January,y1+1)))/dib1;
        sum += daysBetween(to_DateLike(DateAdaptor<ExtDate>::Date(1,January,y2)),d2)/dib2;
        return sum;
    }

    template <class ExtDate> inline
    Time ActualActual<ExtDate>::AFB_Impl::yearFraction(const ExtDate& dd1,
                                              const ExtDate& dd2,
                                              const ExtDate&,
                                              const ExtDate&) const {
        auto d1 = to_DateLike(dd1);
        auto d2 = to_DateLike(dd2);
        if (d1 == d2)
            return 0.0;

        if (d1 > d2)
            return -yearFraction(d2,d1,ExtDate(),ExtDate());

        auto newD2=d2, temp=d2;
        Time sum = 0.0;
        while (temp > d1) {
            temp = newD2 - 1*Years;
            if (temp.dayOfMonth()==28 && temp.month()==2 &&
                DateLike<ExtDate>::isLeap(temp.year())) {
                temp += 1;
            }
            if (temp>=d1) {
                sum += 1.0;
                newD2 = temp;
            }
        }

        Real den = 365.0;

        if (DateLike<ExtDate>::isLeap(newD2.year())) {
            temp = to_DateLike(DateAdaptor<ExtDate>::Date(29, February, newD2.year()));
            if (newD2>temp && d1<=temp)
                den += 1.0;
        } else if (DateLike<ExtDate>::isLeap(d1.year())) {
            temp = to_DateLike(DateAdaptor<ExtDate>::Date(29, February, d1.year()));
            if (newD2>temp && d1<=temp)
                den += 1.0;
        }

        return sum+daysBetween(d1, newD2)/den;
    }

}
