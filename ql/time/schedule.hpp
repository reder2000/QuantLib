/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006, 2007, 2011 Ferdinando Ametrano
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004, 2005, 2006, 2009 StatPro Italia srl

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

/*! \file schedule.hpp
    \brief date schedule
*/

#ifndef quantlib_schedule_hpp
#define quantlib_schedule_hpp

#include <ql/time/calendars/nullcalendar.hpp>
#include <ql/utilities/null.hpp>
#include <ql/time/period.hpp>
#include <ql/time/dategenerationrule.hpp>
#include "ql_errors.hpp"
#include <optional>

namespace QuantLib {

    //! Payment schedule
    /*! \ingroup datetime */
    template <class ExtDate=Date>
    class Schedule {
      public:
        /*! constructor that takes any list of dates, and optionally
            meta information that can be used by client classes. Note
            that neither the list of dates nor the meta information is
            checked for plausibility in any sense. */
        Schedule(
            const std::vector<ExtDate>&,
            const Calendar<ExtDate>& calendar = NullCalendar(),
            BusinessDayConvention convention = Unadjusted,
            const std::optional<BusinessDayConvention>& terminationDateConvention = std::nullopt,
            const std::optional<Period>& tenor = std::nullopt,
            const std::optional<DateGeneration::Rule>& rule = std::nullopt,
            const std::optional<bool>& endOfMonth = std::nullopt,
            const std::vector<bool>& isRegular = std::vector<bool>(0));
        /*! rule based constructor */
        Schedule(ExtDate effectiveDate,
                 const ExtDate& terminationDate,
                 const Period& tenor,
                 const Calendar<ExtDate>& calendar,
                 BusinessDayConvention convention,
                 BusinessDayConvention terminationDateConvention,
                 DateGeneration::Rule rule,
                 bool endOfMonth,
                 const ExtDate& firstDate = ExtDate(),
                 const ExtDate& nextToLastDate = ExtDate());
        Schedule() {}
        //! \name ExtDate access
        //@{
        Size size() const { return dates_.size(); }
        const ExtDate& operator[](Size i) const;
        const ExtDate& at(Size i) const;
        const ExtDate& date(Size i) const;
        ExtDate previousDate(const ExtDate& refDate) const;
        ExtDate nextDate(const ExtDate& refDate) const;
        const std::vector<ExtDate>& dates() const { return dates_; }
        bool hasIsRegular() const;
        bool isRegular(Size i) const;
        const std::vector<bool>& isRegular() const;
        //@}
        //! \name Other inspectors
        //@{
        bool empty() const { return dates_.empty(); }
        const Calendar<ExtDate>& calendar() const;
        const ExtDate& startDate() const;
        const ExtDate& endDate() const;
        bool hasTenor() const;
        const Period& tenor() const;
        BusinessDayConvention businessDayConvention() const;
        bool hasTerminationDateBusinessDayConvention() const;
        BusinessDayConvention terminationDateBusinessDayConvention() const;
        bool hasRule() const;
        DateGeneration::Rule rule() const;
        bool hasEndOfMonth() const;
        bool endOfMonth() const;
        //@}
        //! \name Iterators
        //@{
        typedef typename std::vector<ExtDate>::const_iterator const_iterator;
        const_iterator begin() const { return dates_.begin(); }
        const_iterator end() const { return dates_.end(); }
        const_iterator lower_bound(const ExtDate& d = ExtDate()) const;
        //@}
        //! \name Utilities
        //@{
        //! truncated schedule
        Schedule after(const ExtDate& truncationDate) const;
        Schedule until(const ExtDate& truncationDate) const;
        //@}
      private:
        std::optional<Period> tenor_;
        Calendar<ExtDate> calendar_;
        BusinessDayConvention convention_;
        std::optional<BusinessDayConvention> terminationDateConvention_;
        std::optional<DateGeneration::Rule> rule_;
        std::optional<bool> endOfMonth_;
        ExtDate firstDate_, nextToLastDate_;
        std::vector<ExtDate> dates_;
        std::vector<bool> isRegular_;
    };


    //! helper class
    /*! This class provides a more comfortable interface to the
        argument list of Schedule's constructor.
    */
    template <class ExtDate = Date>
    class MakeSchedule {
      public:
        MakeSchedule();
        MakeSchedule& from(const ExtDate& effectiveDate);
        MakeSchedule& to(const ExtDate& terminationDate);
        MakeSchedule& withTenor(const Period&);
        MakeSchedule& withFrequency(Frequency);
        MakeSchedule& withCalendar(const Calendar<ExtDate>&);
        MakeSchedule& withConvention(BusinessDayConvention);
        MakeSchedule& withTerminationDateConvention(BusinessDayConvention);
        MakeSchedule& withRule(DateGeneration::Rule);
        MakeSchedule& forwards();
        MakeSchedule& backwards();
        MakeSchedule& endOfMonth(bool flag=true);
        MakeSchedule& withFirstDate(const ExtDate& d);
        MakeSchedule& withNextToLastDate(const ExtDate& d);
        operator Schedule<ExtDate>() const;
      private:
        Calendar<ExtDate> calendar_;
        ExtDate effectiveDate_, terminationDate_;
        std::optional<Period> tenor_;
        std::optional<BusinessDayConvention> convention_;
        std::optional<BusinessDayConvention> terminationDateConvention_;
        DateGeneration::Rule rule_;
        bool endOfMonth_;
        ExtDate firstDate_, nextToLastDate_;
    };

    /*! Helper function for returning the date on or before date \p d that is the 20th of the month and obeserves the 
        given date generation \p rule if it is relevant.
    */
    template <class ExtDate = Date>
    ExtDate previousTwentieth(const ExtDate& d, DateGeneration::Rule rule);

    // inline definitions

    template <class ExtDate> inline const ExtDate& Schedule<ExtDate>::date(Size i) const {
        return dates_.at(i);
    }

    template <class ExtDate> inline const ExtDate& Schedule<ExtDate>::operator[](Size i) const {
        #if defined(QL_EXTRA_SAFETY_CHECKS)
        return dates_.at(i);
        #else
        return dates_[i];
        #endif
    }

    template <class ExtDate> inline const ExtDate& Schedule<ExtDate>::at(Size i) const {
        return dates_.at(i);
    }

    template <class ExtDate> inline const Calendar<ExtDate>& Schedule<ExtDate>::calendar() const {
        return calendar_;
    }

    template <class ExtDate> inline const ExtDate& Schedule<ExtDate>::startDate() const {
        return dates_.front();
    }

    template <class ExtDate> inline const ExtDate &Schedule<ExtDate>::endDate() const { return dates_.back(); }

    template <class ExtDate> inline bool Schedule<ExtDate>::hasTenor() const {
        return tenor_ != std::nullopt;
    }

    template <class ExtDate> inline const Period& Schedule<ExtDate>::tenor() const {
        QL_REQUIRE(hasTenor(),
                   "full interface (tenor) not available");
        return *tenor_;
    }

    template <class ExtDate> inline BusinessDayConvention Schedule<ExtDate>::businessDayConvention() const {
        return convention_;
    }

    template <class ExtDate> inline bool
    Schedule<ExtDate>::hasTerminationDateBusinessDayConvention() const {
        return terminationDateConvention_ != std::nullopt;
    }

    template <class ExtDate> inline BusinessDayConvention
    Schedule<ExtDate>::terminationDateBusinessDayConvention() const {
        QL_REQUIRE(hasTerminationDateBusinessDayConvention(),
                   "full interface (termination date bdc) not available");
        return *terminationDateConvention_;
    }

    template <class ExtDate> inline bool Schedule<ExtDate>::hasRule() const {
        return rule_ != std::nullopt;
    }

    template <class ExtDate> inline DateGeneration::Rule Schedule<ExtDate>::rule() const {
        QL_REQUIRE(hasRule(), "full interface (rule) not available");
        return *rule_;
    }

    template <class ExtDate> inline bool Schedule<ExtDate>::hasEndOfMonth() const {
        return endOfMonth_ != std::nullopt;
    }

    template <class ExtDate> inline bool Schedule<ExtDate>::endOfMonth() const {
        QL_REQUIRE(hasEndOfMonth(),
                   "full interface (end of month) not available");
        return *endOfMonth_;
    }

}
#include "schedule.cpp"
#endif
