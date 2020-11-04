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

#ifndef quantlib_business252_day_counter_hpp
#define quantlib_business252_day_counter_hpp

#include "../calendar.hpp"
#include "../calendars/brazil.hpp"
#include "../daycounter.hpp"
#include <utility>
#include <map>
#include <sstream>

namespace QuantLib {

    //! Business/252 day count convention
    /*! \ingroup daycounters */
    template <class Date>
    class Business252 : public DayCounter<Date> {
      private:
        class Impl : public DayCounter<Date>::Impl {
          private:
            Calendar<Date> calendar_;
          public:
            std::string name() const;
            typename date_traits<Date>::serial_type
                  dayCount(const Date& d1,
                                       const Date& d2) const;
            typename date_traits<Date>::Time
            yearFraction(const Date& d1,
                              const Date& d2,
                              const Date&,
                              const Date&) const;
            explicit Impl(const Calendar<Date>& c) : calendar_(c) {}
        };
      public:
        Business252(const Calendar<Date>& c = Brazil<Date>())
        : DayCounter<Date>(std::shared_ptr<typename DayCounter<Date>::Impl>(new Business252::Impl(c))) {}
    };

        namespace detail {

        template <class Date>
        using Cache = std::map<Year, std::map<Month, typename date_traits<Date>::serial_type> >;
        template <class Date>
        using OuterCache = std::map<Year, typename date_traits<Date>::serial_type>;

        template <class Date>
        extern std::map<std::string, Cache<Date> >& monthlyFigures() {
            static std::map<std::string, Cache<Date> > res;
            return res;
        }
        template <class Date>
        inline std::map<std::string, OuterCache<Date> >& yearlyFigures() {
            static std::map<std::string, OuterCache<Date> > res;
            return res;
        }

        template <class Date>
        inline bool sameYear(const Date& d1, const Date& d2) {
            return d1.year() == d2.year();
        }

        template <class Date>
        inline bool sameMonth(const Date& d1, const Date& d2) {
            return d1.year() == d2.year() && d1.month() == d2.month();
        }

        template <class Date>
        inline typename date_traits<Date>::serial_type
        businessDays(Cache<Date>& cache, const Calendar<Date>& calendar, Month month, Year year) {
            if (cache[year][month] == 0) {
                // calculate and store.
                Date d1 = Date(1, month, year);
                Date d2 = date_traits<Date>::plusPeriod( d1 , 1 * Months);
                cache[year][month] = calendar.businessDaysBetween(d1, d2);
            }
            return cache[year][month];
        }

        template <class Date>
        inline typename date_traits<Date>::serial_type businessDays(OuterCache<Date>& outerCache,
                                                                    Cache<Date>& cache,
                                                                    const Calendar<Date>& calendar,
                                                                    Year year) {
            if (outerCache[year] == 0) {
                // calculate and store.
                typename date_traits<Date>:: serial_type total = 0;
                for (int i = 1; i <= 12; ++i) {
                    total += businessDays(cache, calendar, Month(i), year);
                }
                outerCache[year] = total;
            }
            return outerCache[year];
        }

    }

    template <class Date>
    inline std::string Business252<Date>::Impl::name() const {
        std::ostringstream out;
        out << "Business/252(" << calendar_.name() << ")";
        return out.str();
    }

    template <class Date>
    inline typename date_traits<Date>::serial_type
    Business252<Date>::Impl::dayCount(const Date& d1, const Date& d2) const {
        if (detail::sameMonth(d1, d2) || d1 >= d2) {
            // we treat the case of d1 > d2 here, since we'd need a
            // second cache to get it right (our cached figures are
            // for first included, last excluded and might have to be
            // changed going the other way.)
            return calendar_.businessDaysBetween(d1, d2);
        } else if (detail::sameYear(d1, d2)) {
            detail::Cache<Date>& cache = detail::monthlyFigures<Date>()[calendar_.name()];
            typename date_traits<Date>::serial_type total = 0;
            Date d;
            // first, we get to the beginning of next month.
            d = date_traits<Date>::plusPeriod( Date(1, d1.month(), d1.year()) , 1 * Months);
            total += calendar_.businessDaysBetween(d1, d);
            // then, we add any whole months (whose figures might be
            // cached already) in the middle of our period.
            while (!detail::sameMonth(d, d2)) {
                total += detail::businessDays<Date>(cache, calendar_, date_traits<Date>::ql_month(d),
                                                    d.year());
                date_traits<Date>::addSelf(d , 1 * Months);
            }
            // finally, we get to the end of the period.
            total += calendar_.businessDaysBetween(d, d2);
            return total;
        } else {
            detail::Cache<Date>& cache = detail::monthlyFigures<Date>()[calendar_.name()];
            detail::OuterCache<Date>& outerCache = detail::yearlyFigures<Date>()[calendar_.name()];
            typename date_traits<Date>::serial_type total = 0;
            Date d;
            // first, we get to the beginning of next year.
            // The first bit gets us to the end of this month...
            d = date_traits<Date>::plusPeriod( Date(1, d1.month(), d1.year()) , 1 * Months);
            total += calendar_.businessDaysBetween(d1, d);
            // ...then we add any remaining months, possibly cached
            for (int m = int(d1.month()) + 1; m <= 12; ++m) {
                total += detail::businessDays(cache, calendar_, Month(m), d.year());
            }
            // then, we add any whole year in the middle of our period.
            d = Date(1, January, d1.year() + 1);
            while (!detail::sameYear(d, d2)) {
                total += detail::businessDays(outerCache, cache, calendar_, d.year());
                date_traits<Date>::addSelf( d , 1 * Years);
            }
            // finally, we get to the end of the period.
            // First, we add whole months...
            for (int m = 1; m < int(d2.month()); ++m) {
                total += detail::businessDays(cache, calendar_, Month(m), d2.year());
            }
            // ...then the last bit.
            d = Date(1, d2.month(), d2.year());
            total += calendar_.businessDaysBetween(d, d2);
            return total;
        }
    }

    template <class Date>
    inline typename date_traits<Date>::Time Business252<Date>::Impl::yearFraction(
        const Date& d1, const Date& d2, const Date&, const Date&) const {
        return dayCount(d1, d2) / 252.0;
    }

}

#endif
