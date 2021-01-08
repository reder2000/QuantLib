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

#include <ql/time/daycounters/business252.hpp>
#include <map>

namespace QuantLib {

    namespace {

        typedef std::map<Year, std::map<Month, serial_type> > Cache;
        typedef std::map<Year, serial_type> OuterCache;
        
        std::map<std::string, Cache> monthlyFigures_;
        std::map<std::string, OuterCache> yearlyFigures_;
        template <class ExtDate> inline
        bool sameYear(const ExtDate& d1, const ExtDate& d2) {
            return d1.year() == d2.year();
        }
        template <class ExtDate> inline
        bool sameMonth(const ExtDate& d1, const ExtDate& d2) {
            return to_DateLike(d1).year() == to_DateLike(d2).year() && to_DateLike(d1).month() == to_DateLike(d2).month();
        }
        template <class ExtDate> inline
        serial_type businessDays(Cache& cache,
                                       const Calendar<ExtDate>& calendar,
                                       Month month, Year year) {
            if (cache[year][month] == 0) {
                // calculate and store.
                ExtDate d1 = DateAdaptor<ExtDate>::Date(1,month,year);
                ExtDate d2 = to_DateLike(d1) + 1*Months;
                cache[year][month] = calendar.businessDaysBetween(d1, d2);
            }
            return cache[year][month];
        }
        template <class ExtDate> inline
        serial_type businessDays(OuterCache& outerCache,
                                       Cache& cache,
                                       const Calendar<ExtDate>& calendar,
                                       Year year) {
            if (outerCache[year] == 0) {
                // calculate and store.
                serial_type total = 0;
                for (Integer i=1; i<=12; ++i) {
                    total += businessDays(cache,calendar,
                                          Month(i), year);
                }
                outerCache[year] = total;
            }
            return outerCache[year];
        }

    }
    template <class ExtDate> inline
    std::string Business252<ExtDate>::Impl::name() const {
        std::ostringstream out;
        out << "Business/252(" << calendar_.name() << ")";
        return out.str();
    }
    template <class ExtDate> inline
    serial_type Business252<ExtDate>::Impl::dayCount(const ExtDate& dd1,
                                                  const ExtDate& dd2) const {
        auto d1 = to_DateLike(dd1);
        auto d2 = to_DateLike(dd2);
        if (sameMonth(d1.asExtDate(), d2.asExtDate()) || d1 >= d2) {
            // we treat the case of d1 > d2 here, since we'd need a
            // second cache to get it right (our cached figures are
            // for first included, last excluded and might have to be
            // changed going the other way.)
            return calendar_.businessDaysBetween(d1, d2);
        } else if (sameYear(d1,d2)) {
            Cache& cache = monthlyFigures_[calendar_.name()];
            serial_type total = 0;
            // first, we get to the beginning of next month.
            auto dd1 = d1.year_month();
            auto d = to_DateLike(DateAdaptor<ExtDate>::Date(1,dd1.month,dd1.year)) + 1*Months;
            total += calendar_.businessDaysBetween(d1, d);
            // then, we add any whole months (whose figures might be
            // cached already) in the middle of our period.
            while (!sameMonth(d.asExtDate(), d2.asExtDate())) {
                total += businessDays(cache, calendar_,
                                      d.month(), d.year());
                d += 1*Months;
            }
            // finally, we get to the end of the period.
            total += calendar_.businessDaysBetween(d, d2);
            return total;
        } else {
            Cache& cache = monthlyFigures_[calendar_.name()];
            OuterCache& outerCache = yearlyFigures_[calendar_.name()];
            serial_type total = 0;
            // first, we get to the beginning of next year.
            // The first bit gets us to the end of this month...
            auto d = to_DateLike(DateAdaptor<ExtDate>::Date(1, d1.month(), d1.year())) + 1 * Months;
            total += calendar_.businessDaysBetween(d1, d);
            // ...then we add any remaining months, possibly cached
            for (Integer m = Integer(d1.month())+1; m <= 12; ++m) {
                total += businessDays(cache, calendar_,
                                      Month(m), d.year());
            }
            // then, we add any whole year in the middle of our period.
            d = to_DateLike(DateAdaptor<ExtDate>::Date(1, January, d1.year() + 1));
            while (!sameYear(d,d2)) {
                total += businessDays(outerCache, cache,
                                      calendar_, d.year());
                d += 1*Years;
            }
            // finally, we get to the end of the period.
            // First, we add whole months...
            for (Integer m = 1; m<Integer(d2.month()); ++m) {
                total += businessDays(cache, calendar_,
                                      Month(m), d2.year());
            }
            // ...then the last bit.
            d = to_DateLike(DateAdaptor<ExtDate>::Date(1, d2.month(), d2.year()));
            total += calendar_.businessDaysBetween(d, d2);
            return total;
        }
    }
    template <class ExtDate> inline
    Time Business252<ExtDate>::Impl::yearFraction(const ExtDate& d1,
                                         const ExtDate& d2,
                                         const ExtDate&,
                                         const ExtDate&) const {
        return dayCount(d1, d2)/252.0;
    }

}
