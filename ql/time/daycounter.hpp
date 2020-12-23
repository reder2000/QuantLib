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

/*! \file daycounter.hpp
    \brief day counter class
*/
#pragma once
#ifndef quantlib_day_counter_hpp
#define quantlib_day_counter_hpp

#include <ql/time/date.hpp>
#include "ql_errors.hpp"

namespace QuantLib {

    //! day counter class
    /*! This class provides methods for determining the length of a time
        period according to given market convention, both as a number
        of days and as a year fraction.

        The Bridge pattern is used to provide the base behavior of the
        day counter.

        \ingroup datetime
    */template <class ExtDate=Date>
    class DayCounter {
      protected:
        //! abstract base class for day counter implementations
        class Impl {
          public:
            virtual ~Impl() {}
            virtual std::string name() const = 0;
            //! to be overloaded by more complex day counters
            virtual serial_type dayCount(const ExtDate& d1,
                                               const ExtDate& d2) const {
                return (to_DateLike(d2)-to_DateLike(d1));
            }
            virtual Time yearFraction(const ExtDate& d1,
                                      const ExtDate& d2,
                                      const ExtDate& refPeriodStart,
                                      const ExtDate& refPeriodEnd) const = 0;
        };
        std::shared_ptr<Impl> impl_;
        /*! This constructor can be invoked by derived classes which
            define a given implementation.
        */
        explicit DayCounter(const std::shared_ptr<Impl>& impl)
        : impl_(impl) {}
      public:
        /*! The default constructor returns a day counter with a null
            implementation, which is therefore unusable except as a
            placeholder.
        */
        DayCounter() {}
        //! \name DayCounter interface
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
        serial_type dayCount(const ExtDate&,
                                   const ExtDate&) const;
        //! Returns the period between two dates as a fraction of year.
        Time yearFraction(const ExtDate&, const ExtDate&,
                          const ExtDate& refPeriodStart = ExtDate(),
                          const ExtDate& refPeriodEnd = ExtDate()) const;
        //@}
    };

    // comparison based on name

    /*! Returns <tt>true</tt> iff the two day counters belong to the same
        derived class.
        \relates DayCounter
    */template <class ExtDate> inline
    bool operator==(const DayCounter<ExtDate>&,
                    const DayCounter<ExtDate>&);
    template <class ExtDate> inline
    /*! \relates DayCounter */
    bool operator!=(const DayCounter<ExtDate>&,
                    const DayCounter<ExtDate>&);
    template <class ExtDate> inline
    /*! \relates DayCounter */
    std::ostream& operator<<(std::ostream&,
                             const DayCounter<ExtDate>&);


    // inline definitions
    template <class ExtDate> 
    inline bool DayCounter<ExtDate>::empty() const {
        return !impl_;
    }
    template <class ExtDate> 
    inline std::string DayCounter<ExtDate>::name() const {
        QL_REQUIRE(impl_, "no day counter implementation provided");
        return impl_->name();
    }
    template <class ExtDate> 
    inline serial_type DayCounter<ExtDate>::dayCount(const ExtDate& d1,
                                                  const ExtDate& d2) const {
        QL_REQUIRE(impl_, "no day counter implementation provided");
        return impl_->dayCount(d1,d2);
    }
    template <class ExtDate> 
    inline Time DayCounter<ExtDate>::yearFraction(const ExtDate& d1, const ExtDate& d2,
        const ExtDate& refPeriodStart, const ExtDate& refPeriodEnd) const {
            QL_REQUIRE(impl_, "no day counter implementation provided");
            return impl_->yearFraction(d1,d2,refPeriodStart,refPeriodEnd);
    }

    template <class ExtDate> 
    inline bool operator==(const DayCounter<ExtDate>& d1, const DayCounter<ExtDate>& d2) {
        return (d1.empty() && d2.empty())
            || (!d1.empty() && !d2.empty() && d1.name() == d2.name());
    }
    template <class ExtDate> 
    inline bool operator!=(const DayCounter<ExtDate>& d1, const DayCounter<ExtDate>& d2) {
        return !(d1 == d2);
    }
    template <class ExtDate> 
    inline std::ostream& operator<<(std::ostream& out, const DayCounter<ExtDate>&d) {
        return out << d.name();
    }

}

#endif
