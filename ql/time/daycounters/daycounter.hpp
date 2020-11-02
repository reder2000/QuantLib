/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004, 2005, 2006, 2007 StatPro Italia srl

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

/*! \file DayCounter<Date>.hpp
    \brief day counter class
*/

#ifndef quantlib_day_counter_hpp
#define quantlib_day_counter_hpp


#include <ql/errors.hpp>

template <class T>
struct type_traits {
    using serial_type = int;
    using Time = double;
    /*! \relates Date
    \brief Difference in days (including fraction of days) between dates
*/
    static Time daysBetween(const T&, const T&);
    /*! \relates Date
        \brief Difference in days between dates
    */
    static serial_type onlyDaysBetween(const T&, const T&);
    //! whether a date is the last day of its month
    static bool isEndOfMonth(const T&);

    static bool isLeap(int y);
};

namespace QuantLib {

        //! Month names
    /*! \ingroup datetime */
    enum Month {
        January = 1,
        February = 2,
        March = 3,
        April = 4,
        May = 5,
        June = 6,
        July = 7,
        August = 8,
        September = 9,
        October = 10,
        November = 11,
        December = 12,
        Jan = 1,
        Feb = 2,
        Mar = 3,
        Apr = 4,
        Jun = 6,
        Jul = 7,
        Aug = 8,
        Sep = 9,
        Oct = 10,
        Nov = 11,
        Dec = 12
    };

    //! day counter class
    /*! This class provides methods for determining the length of a time
        period according to given market convention, both as a number
        of days and as a year fraction.

        The Bridge pattern is used to provide the base behavior of the
        day counter.

        \ingroup datetime
    */
    template <class Date>
    class DayCounter {
      protected:
        //! abstract base class for day counter implementations
        class Impl {
          public:
            virtual ~Impl() {}
            virtual std::string name() const = 0;
            //! to be overloaded by more complex day counters
            virtual typename type_traits<Date>::serial_type dayCount(const Date& d1,
                                               const Date& d2) const {
                return type_traits<Date>::onlyDaysBetween(d1, d2);
            }
            virtual typename type_traits<Date>::Time
            yearFraction(const Date& d1,
                                      const Date& d2,
                                      const Date& refPeriodStart,
                                      const Date& refPeriodEnd) const = 0;
        };
        std::shared_ptr<Impl> impl_;
        /*! This constructor can be invoked by derived classes which
            define a given implementation.
        */
        explicit DayCounter<Date>(const std::shared_ptr<Impl>& impl)
        : impl_(impl) {}
      public:
        /*! The default constructor returns a day counter with a null
            implementation, which is therefore unusable except as a
            placeholder.
        */
        DayCounter<Date>() {}
        //! \name DayCounter<Date> interface
        //@{
        //!  Returns whether or not the day counter is initialized
        bool empty() const;
        //! Returns the name of the day counter.
        /*! \warning This method is used for output and comparison between
                day counters. It is <b>not</b> meant to be used for writing
                switch-on-type code.
        */
        std::string name() const;
        //! Returns the number of days between two dates.
        typename type_traits<Date>::serial_type dayCount(const Date&,
                                   const Date&) const;
        //! Returns the period between two dates as a fraction of year.
        typename type_traits<Date>::Time yearFraction(const Date&,
                                                      const Date&,
                          const Date& refPeriodStart = Date(),
                          const Date& refPeriodEnd = Date()) const;
        //@}
    };

    // comparison based on name

    /*! Returns <tt>true</tt> iff the two day counters belong to the same
        derived class.
        \relates DayCounter
    */
    template <class Date>
    bool operator==(const DayCounter<Date>&,
                    const DayCounter<Date>&);

    /*! \relates DayCounter<Date> */
    template <class Date>
    bool operator!=(const DayCounter<Date>&,
                    const DayCounter<Date>&);

    /*! \relates DayCounter<Date> */
    template <class Date>
    std::ostream& operator<<(std::ostream&,
                             const DayCounter<Date>&);


    // inline definitions

        template <class Date>
    inline bool DayCounter<Date>::empty() const {
        return !impl_;
    }

        template <class Date>
    inline std::string DayCounter<Date>::name() const {
        QL_REQUIRE(impl_, "no day counter implementation provided");
        return impl_->name();
    }

        template <class Date>
    inline typename type_traits<Date>::serial_type DayCounter<Date>::dayCount(const Date& d1,
                                                  const Date& d2) const {
        QL_REQUIRE(impl_, "no day counter implementation provided");
        return impl_->dayCount(d1,d2);
    }

        template <class Date>
    inline typename type_traits<Date>::Time
    DayCounter<Date>::yearFraction(const Date& d1,
                                         const Date& d2,
        const Date& refPeriodStart, const Date& refPeriodEnd) const {
            QL_REQUIRE(impl_, "no day counter implementation provided");
            return impl_->yearFraction(d1,d2,refPeriodStart,refPeriodEnd);
    }


        template <class Date>
    inline bool operator==(const DayCounter<Date>& d1, const DayCounter<Date>& d2) {
        return (d1.empty() && d2.empty())
            || (!d1.empty() && !d2.empty() && d1.name() == d2.name());
    }

        template <class Date>
    inline bool operator!=(const DayCounter<Date>& d1, const DayCounter<Date>& d2) {
        return !(d1 == d2);
    }

        template <class Date>
    inline std::ostream& operator<<(std::ostream& out, const DayCounter<Date>& d) {
        return out << d.name();
    }

}

#endif
