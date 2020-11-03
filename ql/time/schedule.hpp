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

#include "calendars/nullcalendar.hpp"
#include <ql/utilities/null.hpp>
#include "period.hpp"
#include "dategenerationrule.hpp"
#include <ql/errors.hpp>
#include <boost/optional.hpp>

namespace QuantLib {

    //! Payment schedule
    /*! \ingroup datetime */
    template <class Date>
    class Schedule {
      public:
        /*! constructor that takes any list of dates, and optionally
            meta information that can be used by client classes. Note
            that neither the list of dates nor the meta information is
            checked for plausibility in any sense. */
        Schedule(
            const std::vector<Date>&,
            const Calendar<Date>& calendar = NullCalendar<Date>(),
            BusinessDayConvention convention = Unadjusted,
            const boost::optional<BusinessDayConvention>& terminationDateConvention = boost::none,
            const boost::optional<Period>& tenor = boost::none,
            const boost::optional<DateGeneration::Rule>& rule = boost::none,
            const boost::optional<bool>& endOfMonth = boost::none,
            const std::vector<bool>& isRegular = std::vector<bool>(0));
        /*! rule based constructor */
        Schedule(Date effectiveDate,
                 const Date& terminationDate,
                 const Period& tenor,
                 const Calendar<Date>& calendar,
                 BusinessDayConvention convention,
                 BusinessDayConvention terminationDateConvention,
                 DateGeneration::Rule rule,
                 bool endOfMonth,
                 const Date& firstDate = Date(),
                 const Date& nextToLastDate = Date());
        Schedule() {}
        //! \name Date access
        //@{
        Size size() const { return dates_.size(); }
        const Date& operator[](Size i) const;
        const Date& at(Size i) const;
        const Date& date(Size i) const;
        Date previousDate(const Date& refDate) const;
        Date nextDate(const Date& refDate) const;
        const std::vector<Date>& dates() const { return dates_; }
        bool hasIsRegular() const;
        bool isRegular(Size i) const;
        const std::vector<bool>& isRegular() const;
        //@}
        //! \name Other inspectors
        //@{
        bool empty() const { return dates_.empty(); }
        const Calendar<Date>& calendar() const;
        const Date& startDate() const;
        const Date& endDate() const;
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
        typedef typename std::vector<Date>::const_iterator const_iterator;
        const_iterator begin() const { return dates_.begin(); }
        const_iterator end() const { return dates_.end(); }
        const_iterator lower_bound(const Date& d = Date()) const;
        //@}
        //! \name Utilities
        //@{
        //! truncated schedule
        Schedule after(const Date& truncationDate) const;
        Schedule until(const Date& truncationDate) const;
        //@}
      private:
        boost::optional<Period> tenor_;
        Calendar<Date> calendar_;
        BusinessDayConvention convention_;
        boost::optional<BusinessDayConvention> terminationDateConvention_;
        boost::optional<DateGeneration::Rule> rule_;
        boost::optional<bool> endOfMonth_;
        Date firstDate_, nextToLastDate_;
        std::vector<Date> dates_;
        std::vector<bool> isRegular_;
    };


    //! helper class
    /*! This class provides a more comfortable interface to the
        argument list of Schedule's constructor.
    */
    template <class Date>
    class MakeSchedule {
      public:
        MakeSchedule();
        MakeSchedule& from(const Date& effectiveDate);
        MakeSchedule& to(const Date& terminationDate);
        MakeSchedule& withTenor(const Period&);
        MakeSchedule& withFrequency(Frequency);
        MakeSchedule& withCalendar(const Calendar<Date>&);
        MakeSchedule& withConvention(BusinessDayConvention);
        MakeSchedule& withTerminationDateConvention(BusinessDayConvention);
        MakeSchedule& withRule(DateGeneration::Rule);
        MakeSchedule& forwards();
        MakeSchedule& backwards();
        MakeSchedule& endOfMonth(bool flag=true);
        MakeSchedule& withFirstDate(const Date& d);
        MakeSchedule& withNextToLastDate(const Date& d);
        operator Schedule<Date>() const;
      private:
        Calendar<Date> calendar_;
        Date effectiveDate_, terminationDate_;
        boost::optional<Period> tenor_;
        boost::optional<BusinessDayConvention> convention_;
        boost::optional<BusinessDayConvention> terminationDateConvention_;
        DateGeneration::Rule rule_;
        bool endOfMonth_;
        Date firstDate_, nextToLastDate_;
    };



    // inline definitions

    template <class Date>
    inline const Date& Schedule<Date>::date(Size i) const {
        return dates_.at(i);
    }

    template <class Date>
    inline const Date& Schedule<Date>::operator[](Size i) const {
        #if defined(QL_EXTRA_SAFETY_CHECKS)
        return dates_.at(i);
        #else
        return dates_[i];
        #endif
    }

    template <class Date>
    inline const Date& Schedule<Date>::at(Size i) const {
        return dates_.at(i);
    }

      template <class Date>
    inline const Calendar<Date>& Schedule<Date>::calendar() const {
        return calendar_;
    }

       template <class Date>
    inline const Date& Schedule<Date>::startDate() const {
        return dates_.front();
    }

       template <class Date>
    inline const Date& Schedule<Date>::endDate() const {
        return dates_.back();
    }

       template <class Date>
    inline bool Schedule<Date>::hasTenor() const {
        return tenor_ != boost::none;
    }

      template <class Date>
    inline const Period& Schedule<Date>::tenor() const {
        QL_REQUIRE(hasTenor(),
                   "full interface (tenor) not available");
        return *tenor_;
    }

     template <class Date>
    inline BusinessDayConvention Schedule<Date>::businessDayConvention() const {
        return convention_;
    }

     template <class Date>
    inline bool
    Schedule<Date>::hasTerminationDateBusinessDayConvention() const {
        return terminationDateConvention_ != boost::none;
    }

     template <class Date>
    inline BusinessDayConvention
    Schedule<Date>::terminationDateBusinessDayConvention() const {
        QL_REQUIRE(hasTerminationDateBusinessDayConvention(),
                   "full interface (termination date bdc) not available");
        return *terminationDateConvention_;
    }

      template <class Date>
    inline bool Schedule<Date>::hasRule() const {
        return rule_ != boost::none;
    }

      template <class Date>
    inline DateGeneration::Rule Schedule<Date>::rule() const {
        QL_REQUIRE(hasRule(), "full interface (rule) not available");
        return *rule_;
    }

      template <class Date>
    inline bool Schedule<Date>::hasEndOfMonth() const {
        return endOfMonth_ != boost::none;
    }

      template <class Date>
    inline bool Schedule<Date>::endOfMonth() const {
        QL_REQUIRE(hasEndOfMonth(),
                   "full interface (end of month) not available");
        return *endOfMonth_;
    }

}

#endif
