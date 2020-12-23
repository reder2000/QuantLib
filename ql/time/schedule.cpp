/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006, 2007, 2008, 2010, 2011, 2015 Ferdinando Ametrano
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2009, 2012 StatPro Italia srl

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

#include <ql/time/schedule.hpp>
#include <ql/time/imm.hpp>
#include "ql_settings.hpp"

namespace QuantLib {

    namespace {
        template <class ExtDate> inline
        ExtDate nextTwentieth(const ExtDate& d, DateGeneration::Rule rule) {
            DateLike<ExtDate> result{DateAdaptor<ExtDate>::Date(20, to_DateLike(d).month(), to_DateLike(d).year())};
            if (result < d)
                result += 1*Months;
            if (rule == DateGeneration::TwentiethIMM ||
                rule == DateGeneration::OldCDS ||
                rule == DateGeneration::CDS ||
                rule == DateGeneration::CDS2015) {
                Month m = result.month();
                if (m % 3 != 0) { // not a main IMM nmonth
                    Integer skip = 3 - m%3;
                    result += skip*Months;
                }
            }
            return result;
        }
        inline
        bool allowsEndOfMonth(const Period& tenor) {
            return (tenor.units() == Months || tenor.units() == Years)
                && tenor >= 1*Months;
        }

    }

    template <class ExtDate> inline
    Schedule<ExtDate>::Schedule(const std::vector<ExtDate>& dates,
                       const Calendar<ExtDate>& calendar,
                       BusinessDayConvention convention,
                       const std::optional<BusinessDayConvention>& terminationDateConvention,
                       const std::optional<Period>& tenor,
                       const std::optional<DateGeneration::Rule>& rule,
                       const std::optional<bool>& endOfMonth,
                       const std::vector<bool>& isRegular)
    : tenor_(tenor), calendar_(calendar), convention_(convention),
      terminationDateConvention_(terminationDateConvention), rule_(rule), dates_(dates),
      isRegular_(isRegular) {

        if (tenor != std::nullopt && !allowsEndOfMonth(*tenor))
            endOfMonth_ = false;
        else
            endOfMonth_ = endOfMonth;

        QL_REQUIRE(isRegular_.empty() || isRegular_.size() == dates.size() - 1,
                   "isRegular size ({}) must be zero or equal to the number of dates minus 1 ({})"
            , isRegular_.size() , dates.size() - 1 );
    }
    template <class ExtDate> inline
    Schedule<ExtDate>::Schedule(ExtDate effectiveDate,
                       const ExtDate& terminationDate,
                       const Period& tenor,
                       const Calendar<ExtDate>& cal,
                       BusinessDayConvention convention,
                       BusinessDayConvention terminationDateConvention,
                       DateGeneration::Rule rule,
                       bool endOfMonth,
                       const ExtDate& first,
                       const ExtDate& nextToLast)
    : tenor_(tenor), calendar_(cal), convention_(convention),
      terminationDateConvention_(terminationDateConvention), rule_(rule),
      endOfMonth_(allowsEndOfMonth(tenor) ? endOfMonth : false),
      firstDate_(to_DateLike(first)==effectiveDate ? ExtDate() : first),
      nextToLastDate_(to_DateLike(nextToLast)==terminationDate ? ExtDate() : nextToLast)
    {
        // sanity checks
        QL_REQUIRE(to_DateLike(terminationDate) != ExtDate(), "null termination date");

        // in many cases (e.g. non-expired bonds) the effective date is not
        // really necessary. In these cases a decent placeholder is enough
        if (to_DateLike(effectiveDate)==ExtDate() && to_DateLike(first)==ExtDate()
                                  && rule==DateGeneration::Backward) {
            ExtDate evalDate = Settings<ExtDate>::instance().evaluationDate();
            QL_REQUIRE(to_DateLike(evalDate) < terminationDate, "null effective date");
            Natural y;
            if (to_DateLike(nextToLast) != ExtDate()) {
                y = (to_DateLike(nextToLast) - to_DateLike(evalDate))/366 + 1;
                effectiveDate = to_DateLike(nextToLast) - y*Years;
            } else {
                y = (to_DateLike(terminationDate) - to_DateLike(evalDate))/366 + 1;
                effectiveDate = to_DateLike(terminationDate) - y*Years;
            }
        } else
            QL_REQUIRE(to_DateLike(effectiveDate) != ExtDate(), "null effective date");

        QL_REQUIRE(to_DateLike(effectiveDate) < terminationDate,
                   "effective date ({}) later than or equal to termination date ({})" , effectiveDate , terminationDate );

        if (tenor.length()==0)
            rule_ = DateGeneration::Zero;
        else
            QL_REQUIRE(tenor.length()>0,
                       "non positive tenor ({}) not allowed", tenor);

        if (to_DateLike(firstDate_) != ExtDate()) {
            switch (*rule_) {
              case DateGeneration::Backward:
              case DateGeneration::Forward:
                QL_REQUIRE(to_DateLike(firstDate_) > effectiveDate &&
                           to_DateLike(firstDate_) <= terminationDate,
                           "first date () out of effective-termination date range [{},{}]" , firstDate_ , effectiveDate , terminationDate );
                // we should ensure that the above condition is still
                // verified after adjustment
                break;
              case DateGeneration::ThirdWednesday:
                  QL_REQUIRE(IMM<ExtDate>::isIMMdate(firstDate_, false),
                             "first date ({}) is not an IMM date", firstDate_ );
                break;
              case DateGeneration::Zero:
              case DateGeneration::Twentieth:
              case DateGeneration::TwentiethIMM:
              case DateGeneration::OldCDS:
              case DateGeneration::CDS:
              case DateGeneration::CDS2015:
                QL_FAIL("first date incompatible with {} date generation rule",*rule_);
              default:
                QL_FAIL("unknown rule ({})",*rule_);
            }
        }
        if (to_DateLike(nextToLastDate_) != ExtDate()) {
            switch (*rule_) {
              case DateGeneration::Backward:
              case DateGeneration::Forward:
                QL_REQUIRE(to_DateLike(nextToLastDate_) >= effectiveDate &&
                           to_DateLike(nextToLastDate_) < terminationDate,
                           "next to last date ({}) out of effective-termination date range [{},{}]" , nextToLastDate_ , effectiveDate , terminationDate );
                // we should ensure that the above condition is still
                // verified after adjustment
                break;
              case DateGeneration::ThirdWednesday:
                QL_REQUIRE(IMM<ExtDate>::isIMMdate(nextToLastDate_, false),
                           "next-to-last date ({}) is not an IMM date", nextToLastDate_ );
                break;
              case DateGeneration::Zero:
              case DateGeneration::Twentieth:
              case DateGeneration::TwentiethIMM:
              case DateGeneration::OldCDS:
              case DateGeneration::CDS:
              case DateGeneration::CDS2015:
                QL_FAIL("next to last date incompatible with {} date generation rule",*rule_);
              default:
                  QL_FAIL("unknown rule ({})", *rule_);
            }
        }


        // calendar needed for endOfMonth adjustment
        Calendar<ExtDate> nullCalendar = NullCalendar<ExtDate>();
        Integer periods = 1;
        ExtDate seed, exitDate;
        switch (*rule_) {

          case DateGeneration::Zero:
            tenor_ = 0*Years;
            dates_.push_back(effectiveDate);
            dates_.push_back(terminationDate);
            isRegular_.push_back(true);
            break;

          case DateGeneration::Backward:

            dates_.push_back(terminationDate);

            seed = terminationDate;
            if (to_DateLike(nextToLastDate_) != ExtDate()) {
                dates_.insert(dates_.begin(), nextToLastDate_);
                ExtDate temp = nullCalendar.advance(seed,
                    -periods*(*tenor_), convention, *endOfMonth_);
                if (to_DateLike(temp)!=nextToLastDate_)
                    isRegular_.insert(isRegular_.begin(), false);
                else
                    isRegular_.insert(isRegular_.begin(), true);
                seed = nextToLastDate_;
            }

            exitDate = effectiveDate;
            if (to_DateLike(firstDate_) != ExtDate())
                exitDate = firstDate_;

            for (;;) {
                ExtDate temp = nullCalendar.advance(seed,
                    -periods*(*tenor_), convention, *endOfMonth_);
                if (to_DateLike(temp) < exitDate) {
                    if (to_DateLike(firstDate_) != ExtDate() &&
                        (to_DateLike(calendar_.adjust(dates_.front(),convention))!=
                         calendar_.adjust(firstDate_,convention))) {
                        dates_.insert(dates_.begin(), firstDate_);
                        isRegular_.insert(isRegular_.begin(), false);
                    }
                    break;
                } else {
                    // skip dates that would result in duplicates
                    // after adjustment
                    if (to_DateLike(calendar_.adjust(dates_.front(),convention))!=
                        calendar_.adjust(temp,convention)) {
                        dates_.insert(dates_.begin(), temp);
                        isRegular_.insert(isRegular_.begin(), true);
                    }
                    ++periods;
                }
            }

            if (to_DateLike(calendar_.adjust(dates_.front(),convention))!=
                calendar_.adjust(effectiveDate,convention)) {
                dates_.insert(dates_.begin(), effectiveDate);
                isRegular_.insert(isRegular_.begin(), false);
            }
            break;

          case DateGeneration::Twentieth:
          case DateGeneration::TwentiethIMM:
          case DateGeneration::ThirdWednesday:
          case DateGeneration::OldCDS:
          case DateGeneration::CDS:
          case DateGeneration::CDS2015:
            QL_REQUIRE(!*endOfMonth_,
                         "endOfMonth convention incompatible with {} date generation rule", *rule_);
          // fall through
          case DateGeneration::Forward:

            if (*rule_ == DateGeneration::CDS || *rule_ == DateGeneration::CDS2015) {
                ExtDate prev20th = previousTwentieth(effectiveDate, *rule_);
                if (to_DateLike(calendar_.adjust(prev20th, convention)) > effectiveDate) {
                    dates_.push_back(to_DateLike(prev20th) - 3 * Months);
                    isRegular_.push_back(true);
                }
                dates_.push_back(prev20th);
            } else {
                dates_.push_back(effectiveDate);
            }

            seed = dates_.back();

            if (to_DateLike(firstDate_)!=ExtDate()) {
                dates_.push_back(firstDate_);
                ExtDate temp = nullCalendar.advance(seed, periods*(*tenor_),
                                                 convention, *endOfMonth_);
                if (to_DateLike(temp)!=firstDate_)
                    isRegular_.push_back(false);
                else
                    isRegular_.push_back(true);
                seed = firstDate_;
            } else if (*rule_ == DateGeneration::Twentieth ||
                       *rule_ == DateGeneration::TwentiethIMM ||
                       *rule_ == DateGeneration::OldCDS ||
                       *rule_ == DateGeneration::CDS ||
                       *rule_ == DateGeneration::CDS2015) {
                ExtDate next20th = nextTwentieth(effectiveDate, *rule_);
                if (*rule_ == DateGeneration::OldCDS) {
                    // distance rule inforced in natural days
                    static const serial_type stubDays = 30;
                    if (to_DateLike(next20th) - to_DateLike(effectiveDate) < stubDays) {
                        // +1 will skip this one and get the next
                        next20th = nextTwentieth((to_DateLike(next20th) + 1).asExtDate(), *rule_);
                    }
                }
                if (to_DateLike(next20th) != effectiveDate) {
                    dates_.push_back(next20th);
                    isRegular_.push_back(*rule_ == DateGeneration::CDS || *rule_ == DateGeneration::CDS2015);
                    seed = next20th;
                }
            }

            exitDate = terminationDate;
            if (to_DateLike(nextToLastDate_) != ExtDate())
                exitDate = nextToLastDate_;
            for (;;) {
                ExtDate temp = nullCalendar.advance(seed, periods*(*tenor_),
                                                 convention, *endOfMonth_);
                if (to_DateLike(temp) > exitDate) {
                    if (to_DateLike(nextToLastDate_) != ExtDate() &&
                        (to_DateLike(calendar_.adjust(dates_.back(),convention))!=
                         calendar_.adjust(nextToLastDate_,convention))) {
                        dates_.push_back(nextToLastDate_);
                        isRegular_.push_back(false);
                    }
                    break;
                } else {
                    // skip dates that would result in duplicates
                    // after adjustment
                    if (to_DateLike(calendar_.adjust(dates_.back(),convention))!=
                        calendar_.adjust(temp,convention)) {
                        dates_.push_back(temp);
                        isRegular_.push_back(true);
                    }
                    ++periods;
                }
            }

            if (to_DateLike(calendar_.adjust(dates_.back(),terminationDateConvention))!=
                calendar_.adjust(terminationDate,terminationDateConvention)) {
                if (*rule_ == DateGeneration::Twentieth ||
                    *rule_ == DateGeneration::TwentiethIMM ||
                    *rule_ == DateGeneration::OldCDS ||
                    *rule_ == DateGeneration::CDS ||
                    *rule_ == DateGeneration::CDS2015) {
                    dates_.push_back(nextTwentieth(terminationDate, *rule_));
                    isRegular_.push_back(true);
                } else {
                    dates_.push_back(terminationDate);
                    isRegular_.push_back(false);
                }
            }

            break;

          default:
              QL_FAIL("unknown rule ({})", Integer(*rule_));
        }

        // adjustments
        if (*rule_==DateGeneration::ThirdWednesday)
            for (Size i=1; i<dates_.size()-1; ++i)
                dates_[i] = DateLike<ExtDate>::nthWeekday(3, Wednesday,
                                             to_DateLike(dates_[i]).month(),
                                             to_DateLike(dates_[i]).year());

        if (*endOfMonth_ && calendar_.isEndOfMonth(seed)) {
            // adjust to end of month
            if (convention == Unadjusted) {
                for (Size i=1; i<dates_.size()-1; ++i)
                    dates_[i] = DateLike<ExtDate>::endOfMonth(to_DateLike(dates_[i]));
            } else {
                for (Size i=1; i<dates_.size()-1; ++i)
                    dates_[i] = calendar_.endOfMonth(dates_[i]);
            }
            ExtDate d1 = dates_.front(), d2 = dates_.back();
            if (terminationDateConvention != Unadjusted) {
                d1 = calendar_.endOfMonth(dates_.front());
                d2 = calendar_.endOfMonth(dates_.back());
            } else {
                // the termination date is the first if going backwards,
                // the last otherwise.
                if (*rule_ == DateGeneration::Backward)
                    d2 = DateLike<ExtDate>::endOfMonth(to_DateLike(dates_.back()));
                else
                    d1 = DateLike<ExtDate>::endOfMonth(to_DateLike(dates_.front()));
            }
            // if the eom adjustment leads to a single date schedule
            // we do not apply it
            if(to_DateLike(d1) != d2) {
                dates_.front() = d1;
                dates_.back() = d2;
            }
        } else {
            // first date not adjusted for old CDS schedules
            if (*rule_ != DateGeneration::OldCDS)
                dates_[0] = calendar_.adjust(dates_[0], convention);
            for (Size i=1; i<dates_.size()-1; ++i)
                dates_[i] = calendar_.adjust(dates_[i], convention);

            // termination date is NOT adjusted as per ISDA
            // specifications, unless otherwise specified in the
            // confirmation of the deal or unless we're creating a CDS
            // schedule
            if (terminationDateConvention != Unadjusted
                && *rule_ != DateGeneration::CDS
                && *rule_ != DateGeneration::CDS2015) {
                dates_.back() = calendar_.adjust(dates_.back(),
                                                 terminationDateConvention);
            }
        }

        // Final safety checks to remove extra next-to-last date, if
        // necessary.  It can happen to be equal or later than the end
        // date due to EOM adjustments (see the Schedule test suite
        // for an example).
        if (dates_.size() >= 2 && to_DateLike(dates_[dates_.size()-2]) >= dates_.back()) {
            // there might be two dates only, then isRegular_ has size one
            if (isRegular_.size() >= 2) {
                isRegular_[isRegular_.size() - 2] =
                    (to_DateLike(dates_[dates_.size() - 2]) == dates_.back());
            }
            dates_[dates_.size() - 2] = dates_.back();
            dates_.pop_back();
            isRegular_.pop_back();
        }
        if (dates_.size() >= 2 && to_DateLike(dates_[1]) <= dates_.front()) {
            isRegular_[1] =
                (to_DateLike(dates_[1]) == dates_.front());
            dates_[1] = dates_.front();
            dates_.erase(dates_.begin());
            isRegular_.erase(isRegular_.begin());
        }

        QL_ENSURE(dates_.size()>1,
            "degenerate single date ({}) schedule" 
            "\n seed date: {}"
            "\n exit date: {}"
            "\n effective date: {}"
            "\n first date: {}"
            "\n next to last date: {}"
            "\n termination date: {}"
            "\n generation rule: {} end of month: " , dates_[0]  ,seed,
                  exitDate,
                  effectiveDate,
                  first,
                  nextToLast,
                  terminationDate,
                  *rule_,
                  *endOfMonth_);

    }
    template <class ExtDate> inline
    Schedule<ExtDate> Schedule<ExtDate>::after(const ExtDate& truncationDate) const {
        Schedule result = *this;

        QL_REQUIRE(truncationDate < result.dates_.back(),
                   "truncation date {} "
                   " must be before the last schedule date {}",
                   truncationDate
             ,
            result.dates_.back());
        if (truncationDate > result.dates_[0]) {
            // remove earlier dates
            while (result.dates_[0] < truncationDate) {
                result.dates_.erase(result.dates_.begin());
                if (!result.isRegular_.empty())
                    result.isRegular_.erase(result.isRegular_.begin());
            }

            // add truncationDate if missing
            if (truncationDate != result.dates_.front()) {
                result.dates_.insert(result.dates_.begin(), truncationDate);
                result.isRegular_.insert(result.isRegular_.begin(), false);
                result.terminationDateConvention_ = Unadjusted;
            }
            else {
                result.terminationDateConvention_ = convention_;
            }

            if (result.nextToLastDate_ <= truncationDate)
                result.nextToLastDate_ = ExtDate();
            if (result.firstDate_ <= truncationDate)
                result.firstDate_ = ExtDate();
        }

        return result;
    }
    template <class ExtDate> inline
    Schedule<ExtDate> Schedule<ExtDate>::until(const ExtDate& truncationDate) const {
        Schedule result = *this;

        QL_REQUIRE(truncationDate>result.dates_[0],
                   "truncation date {} must be later than schedule first date {}",
                   truncationDate ,
                   result.dates_[0]);
        if (truncationDate<result.dates_.back()) {
            // remove later dates
            while (result.dates_.back()>truncationDate) {
                result.dates_.pop_back();
                if(!result.isRegular_.empty())
                    result.isRegular_.pop_back();
            }

            // add truncationDate if missing
            if (truncationDate!=result.dates_.back()) {
                result.dates_.push_back(truncationDate);
                result.isRegular_.push_back(false);
                result.terminationDateConvention_ = Unadjusted;
            } else {
                result.terminationDateConvention_ = convention_;
            }

            if (result.nextToLastDate_>=truncationDate)
                result.nextToLastDate_ = ExtDate();
            if (result.firstDate_>=truncationDate)
                result.firstDate_ = ExtDate();
        }

        return result;
    }
    template <class ExtDate> inline
    typename std::vector<ExtDate>::const_iterator
    Schedule<ExtDate>::lower_bound(const ExtDate& refDate) const {
        ExtDate d = (refDate==ExtDate() ?
                  Settings<ExtDate>::instance().evaluationDate() :
                  refDate);
        return std::lower_bound(dates_.begin(), dates_.end(), d);
    }
    template <class ExtDate> inline
    ExtDate Schedule<ExtDate>::nextDate(const ExtDate& refDate) const {
        auto res = lower_bound(refDate);
        if (res!=dates_.end())
            return *res;
        else
            return ExtDate();
    }
    template <class ExtDate> inline
    ExtDate Schedule<ExtDate>::previousDate(const ExtDate& refDate) const {
        auto res = lower_bound(refDate);
        if (res!=dates_.begin())
            return *(--res);
        else
            return ExtDate();
    }
    template <class ExtDate> inline
    bool Schedule<ExtDate>::hasIsRegular() const { return !isRegular_.empty(); }
    template <class ExtDate> inline
    bool Schedule<ExtDate>::isRegular(Size i) const {
        QL_REQUIRE(hasIsRegular(),
                   "full interface (isRegular) not available");
        QL_REQUIRE(i<=isRegular_.size() && i>0,
                   "index ({}) must be in [1,{}]" , i , isRegular_.size() );
        return isRegular_[i-1];
    }
    template <class ExtDate> inline
    const std::vector<bool>& Schedule<ExtDate>::isRegular() const {
        QL_REQUIRE(!isRegular_.empty(), "full interface (isRegular) not available");
        return isRegular_;
    }
    template <class ExtDate> inline
    MakeSchedule<ExtDate>::MakeSchedule()
    : rule_(DateGeneration::Backward), endOfMonth_(false) {}
    template <class ExtDate> inline
    MakeSchedule<ExtDate>& MakeSchedule<ExtDate>::from(const ExtDate& effectiveDate) {
        effectiveDate_ = effectiveDate;
        return *this;
    }
    template <class ExtDate> inline
    MakeSchedule<ExtDate>& MakeSchedule<ExtDate>::to(const ExtDate& terminationDate) {
        terminationDate_ = terminationDate;
        return *this;
    }
    template <class ExtDate> inline
    MakeSchedule<ExtDate>& MakeSchedule<ExtDate>::withTenor(const Period& tenor) {
        tenor_ = tenor;
        return *this;
    }
    template <class ExtDate> inline
    MakeSchedule<ExtDate>& MakeSchedule<ExtDate>::withFrequency(Frequency frequency) {
        tenor_ = Period(frequency);
        return *this;
    }
    template <class ExtDate> inline
    MakeSchedule<ExtDate>& MakeSchedule<ExtDate>::withCalendar(const Calendar<ExtDate>& calendar) {
        calendar_ = calendar;
        return *this;
    }
    template <class ExtDate> inline
    MakeSchedule<ExtDate>& MakeSchedule<ExtDate>::withConvention(BusinessDayConvention conv) {
        convention_ = conv;
        return *this;
    }
    template <class ExtDate> inline
    MakeSchedule<ExtDate>& MakeSchedule<ExtDate>::withTerminationDateConvention(
                                                BusinessDayConvention conv) {
        terminationDateConvention_ = conv;
        return *this;
    }
    template <class ExtDate> inline
    MakeSchedule<ExtDate>& MakeSchedule<ExtDate>::withRule(DateGeneration::Rule r) {
        rule_ = r;
        return *this;
    }
    template <class ExtDate> inline
    MakeSchedule<ExtDate>& MakeSchedule<ExtDate>::forwards() {
        rule_ = DateGeneration::Forward;
        return *this;
    }
    template <class ExtDate> inline
    MakeSchedule<ExtDate>& MakeSchedule<ExtDate>::backwards() {
        rule_ = DateGeneration::Backward;
        return *this;
    }
    template <class ExtDate> inline
    MakeSchedule<ExtDate>& MakeSchedule<ExtDate>::endOfMonth(bool flag) {
        endOfMonth_ = flag;
        return *this;
    }
    template <class ExtDate> inline
    MakeSchedule<ExtDate>& MakeSchedule<ExtDate>::withFirstDate(const ExtDate& d) {
        firstDate_ = d;
        return *this;
    }
    template <class ExtDate> inline
    MakeSchedule<ExtDate>& MakeSchedule<ExtDate>::withNextToLastDate(const ExtDate& d) {
        nextToLastDate_ = d;
        return *this;
    }
    template <class ExtDate> inline
    MakeSchedule<ExtDate>::operator Schedule<ExtDate>() const {
        // check for mandatory arguments
        QL_REQUIRE(to_DateLike(effectiveDate_) != ExtDate(), "effective date not provided");
        QL_REQUIRE(to_DateLike(terminationDate_) != ExtDate(), "termination date not provided");
        QL_REQUIRE(tenor_, "tenor/frequency not provided");

        // set dynamic defaults:
        BusinessDayConvention convention;
        // if a convention was set, we use it.
        if (convention_) { // NOLINT(readability-implicit-bool-conversion)
            convention = *convention_;
        } else {
            if (!calendar_.empty()) {
                // ...if we set a calendar, we probably want it to be used;
                convention = Following;
            } else {
                // if not, we don't care.
                convention = Unadjusted;
            }
        }

        BusinessDayConvention terminationDateConvention;
        // if set explicitly, we use it;
        if (terminationDateConvention_) { // NOLINT(readability-implicit-bool-conversion)
            terminationDateConvention = *terminationDateConvention_;
        } else {
            // Unadjusted as per ISDA specification
            terminationDateConvention = convention;
        }

        Calendar<ExtDate> calendar = calendar_;
        // if no calendar was set...
        if (calendar.empty()) {
            // ...we use a null one.
            calendar = NullCalendar<eDate>();
        }

        return Schedule<ExtDate>(effectiveDate_, terminationDate_, *tenor_, calendar,
                        convention, terminationDateConvention,
                        rule_, endOfMonth_, firstDate_, nextToLastDate_);
    }
    template <class ExtDate> inline
    ExtDate previousTwentieth(const ExtDate& d, DateGeneration::Rule rule) {
        DateLike<ExtDate> result{DateAdaptor<ExtDate>::Date(20, to_DateLike(d).month(), to_DateLike(d).year())};
        if (result > d)
            result -= 1 * Months;
        if (rule == DateGeneration::TwentiethIMM ||
            rule == DateGeneration::OldCDS ||
            rule == DateGeneration::CDS ||
            rule == DateGeneration::CDS2015) {
            Month m = result.month();
            if (m % 3 != 0) { // not a main IMM nmonth
                Integer skip = m % 3;
                result -= skip * Months;
            }
        }
        return result;
    }

}
