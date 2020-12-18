/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003 RiskMap srl
 Copyright (C) 2007 StatPro Italia srl
 Copyright (C) 2020 Piotr Siejda

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

#include <ql/time/calendars/jointcalendar.hpp>
#include "../ql_errors.hpp"
#include <sstream>

namespace QuantLib {
    template <class ExtDate> inline
    JointCalendar<ExtDate>::Impl::Impl(const Calendar<ExtDate>& c1,
                              const Calendar<ExtDate>& c2,
                              JointCalendarRule r)
    : rule_(r), calendars_(2) {
        calendars_[0] = c1;
        calendars_[1] = c2;
    }

    template <class ExtDate> inline
    JointCalendar<ExtDate>::Impl::Impl(const Calendar<ExtDate>& c1,
                              const Calendar<ExtDate>& c2,
                              const Calendar<ExtDate>& c3,
                              JointCalendarRule r)
    : rule_(r), calendars_(3) {
        calendars_[0] = c1;
        calendars_[1] = c2;
        calendars_[2] = c3;
    }
    template <class ExtDate> inline
    JointCalendar<ExtDate>::Impl::Impl(const Calendar<ExtDate>& c1,
                              const Calendar<ExtDate>& c2,
                              const Calendar<ExtDate>& c3,
                              const Calendar<ExtDate>& c4,
                              JointCalendarRule r)
    : rule_(r), calendars_(4) {
        calendars_[0] = c1;
        calendars_[1] = c2;
        calendars_[2] = c3;
        calendars_[3] = c4;
    }
    template <class ExtDate> inline
    JointCalendar<ExtDate>::Impl::Impl(const std::vector<Calendar<ExtDate>> &cv,
                              JointCalendarRule r)
    : rule_(r), calendars_(cv){
    }
    template <class ExtDate> inline
    std::string JointCalendar<ExtDate>::Impl::name() const {
        std::ostringstream out;
        switch (rule_) {
          case JoinHolidays:
            out << "JoinHolidays(";
            break;
          case JoinBusinessDays:
            out << "JoinBusinessDays(";
            break;
          default:
            QL_FAIL("unknown joint calendar rule");
        }
        out << calendars_.front().name();
        //std::vector<Calendar<ExtDate>>::const_iterator i;
        for (auto i=calendars_.begin()+1; i!=calendars_.end(); ++i)
            out << ", " << i->name();
        out << ")";
        return out.str();
    }
    template <class ExtDate> inline
    bool JointCalendar<ExtDate>::Impl::isWeekend(Weekday w) const {
        //std::vector<Calendar<ExtDate> >::const_iterator i;
        switch (rule_) {
          case JoinHolidays:
            for (auto i=calendars_.begin(); i!=calendars_.end(); ++i) {
                if (i->isWeekend(w))
                    return true;
            }
            return false;
          case JoinBusinessDays:
            for (auto i=calendars_.begin(); i!=calendars_.end(); ++i) {
                if (!i->isWeekend(w))
                    return false;
            }
            return true;
          default:
            QL_FAIL("unknown joint calendar rule");
        }
    }
    template <class ExtDate> inline
    bool JointCalendar<ExtDate>::Impl::isBusinessDay(const ExtDate& date) const {
        //std::vector<Calendar<ExtDate>>::const_iterator i;
        switch (rule_) {
          case JoinHolidays:
            for (auto i=calendars_.begin(); i!=calendars_.end(); ++i) {
                if (i->isHoliday(date))
                    return false;
            }
            return true;
          case JoinBusinessDays:
            for (auto i=calendars_.begin(); i!=calendars_.end(); ++i) {
                if (i->isBusinessDay(date))
                    return true;
            }
            return false;
          default:
            QL_FAIL("unknown joint calendar rule");
        }
    }

    template <class ExtDate> inline
    JointCalendar<ExtDate>::JointCalendar(const Calendar<ExtDate>& c1,
                                 const Calendar<ExtDate>& c2,
                                 JointCalendarRule r) {
        this->impl_ = std::shared_ptr<Calendar<ExtDate>::Impl>(
                                            new JointCalendar<ExtDate>::Impl(c1,c2,r));
    }
    template <class ExtDate> inline
    JointCalendar<ExtDate>::JointCalendar(const Calendar<ExtDate>& c1,
                                 const Calendar<ExtDate>& c2,
                                 const Calendar<ExtDate>& c3,
                                 JointCalendarRule r) {
        this->impl_ = std::shared_ptr<Calendar<ExtDate>::Impl>(
                                         new JointCalendar<ExtDate>::Impl(c1,c2,c3,r));
    }
    template <class ExtDate> inline
    JointCalendar<ExtDate>::JointCalendar(const Calendar<ExtDate>& c1,
                                 const Calendar<ExtDate>& c2,
                                 const Calendar<ExtDate>& c3,
                                 const Calendar<ExtDate>& c4,
                                 JointCalendarRule r) {
        this->impl_ = std::shared_ptr<Calendar<ExtDate>::Impl>(
                                      new JointCalendar<ExtDate>::Impl(c1,c2,c3,c4,r));
    }
    template <class ExtDate> inline
    JointCalendar<ExtDate>::JointCalendar(const std::vector<Calendar<ExtDate>> &cv,
                                 JointCalendarRule r) {
        this->impl_ = std::shared_ptr<Calendar<ExtDate>::Impl>(
                                      new JointCalendar<ExtDate>::Impl(cv,r));
    }

}
