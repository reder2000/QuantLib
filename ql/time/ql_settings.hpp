/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007, 2011 Ferdinando Ametrano
 Copyright (C) 2007 François du Vignaud
 Copyright (C) 2004, 2005, 2007, 2009 StatPro Italia srl

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

/*! \file settings.hpp
    \brief global repository for run-time library settings
*/
#pragma once
#ifndef quantlib_settings_hpp
#define quantlib_settings_hpp

#include "ql_patterns_singleton.hpp"
#include <ql/time/date.hpp>
#include "ql_utilities_observablevalue.hpp"
#include <optional>

#if __GNUC__
#pragma GCC diagnostic ignored "-Wnon-template-friend"
#endif

namespace QuantLib {

    //! global repository for run-time library settings
    template <class ExtDate=Date>
    class Settings : public Singleton<Settings<ExtDate>> {
        friend class Singleton<Settings<ExtDate>>;
      private:
        Settings();
        class DateProxy : public ObservableValue<ExtDate> {
          public:
            DateProxy();
            DateProxy& operator=(const ExtDate&);
            operator ExtDate() const;
        };
        friend std::ostream& operator<<(std::ostream&, const DateProxy&);
      public:
        //! the date at which pricing is to be performed.
        /*! Client code can inspect the evaluation date, as in:
            \code
            ExtDate d = Settings<ExtDate>::instance().evaluationDate();
            \endcode
            where today's date is returned if the evaluation date is
            set to the null date (its default value;) can set it to a
            new value, as in:
            \code
            Settings<ExtDate>::instance().evaluationDate() = d;
            \endcode
            and can register with it, as in:
            \code
            registerWith(Settings<ExtDate>::instance().evaluationDate());
            \endcode
            to be notified when it is set to a new value.
            \warning a notification is not sent when the evaluation
                     date changes for natural causes---i.e., a date
                     was not explicitly set (which results in today's
                     date being used for pricing) and the current date
                     changes as the clock strikes midnight.
        */
        DateProxy& evaluationDate();
        const DateProxy& evaluationDate() const;

        /*! Call this to prevent the evaluation date to change at
            midnight (and, incidentally, to gain quite a bit of
            performance.)  If no evaluation date was previously set,
            it is equivalent to setting the evaluation date to
            ExtDate::todaysDate(); if an evaluation date other than
            ExtDate() was already set, it has no effect.
        */
        void anchorEvaluationDate();
        /*! Call this to reset the evaluation date to
            ExtDate::todaysDate() and allow it to change at midnight.  It
            is equivalent to setting the evaluation date to ExtDate().
            This comes at the price of losing some performance, since
            the evaluation date is re-evaluated each time it is read.
        */
        void resetEvaluationDate();

        /*! This flag specifies whether or not Events occurring on the reference
            date should, by default, be taken into account as not happened yet.
            It can be overridden locally when calling the Event::hasOccurred
            method.
        */
        bool& includeReferenceDateEvents();
        bool includeReferenceDateEvents() const;

        /*! If set, this flag specifies whether or not CashFlows
            occurring on today's date should enter the NPV.  When the
            NPV date (i.e., the date at which the cash flows are
            discounted) equals today's date, this flag overrides the
            behavior chosen for includeReferenceDate. It cannot be overridden
            locally when calling the CashFlow::hasOccurred method.
        */
        std::optional<bool>& includeTodaysCashFlows();
        std::optional<bool> includeTodaysCashFlows() const;

        bool& enforcesTodaysHistoricFixings();
        bool enforcesTodaysHistoricFixings() const;
      private:
        DateProxy evaluationDate_;
        bool includeReferenceDateEvents_;
        std::optional<bool> includeTodaysCashFlows_;
        bool enforcesTodaysHistoricFixings_;
    };


    // helper class to temporarily and safely change the settings
    template <class ExtDate=Date>
    class SavedSettings {
      public:
        SavedSettings();
        ~SavedSettings();
      private:
        ExtDate evaluationDate_;
        bool includeReferenceDateEvents_;
        std::optional<bool> includeTodaysCashFlows_;
        bool enforcesTodaysHistoricFixings_;
    };


    // inline
    template <class ExtDate>
    inline Settings<ExtDate>::DateProxy::operator ExtDate() const {
        if (to_DateLike(this->value()) == ExtDate())
            return DateLike<ExtDate>::todaysDate();
        else
            return this->value();
    }
    template <class ExtDate>
    inline typename Settings<ExtDate>::DateProxy& Settings<ExtDate>::DateProxy::operator=(const ExtDate& d) {
        ObservableValue<ExtDate>::operator=(d);
        return *this;
    }
    template <class ExtDate>
    inline typename Settings<ExtDate>::DateProxy& Settings<ExtDate>::evaluationDate() {
        return evaluationDate_;
    }
    template <class ExtDate>
    inline const typename Settings<ExtDate>::DateProxy& Settings<ExtDate>::evaluationDate() const {
        return evaluationDate_;
    }
    template <class ExtDate>
    inline bool& Settings<ExtDate>::includeReferenceDateEvents() {
        return includeReferenceDateEvents_;
    }
    template <class ExtDate>
    inline bool Settings<ExtDate>::includeReferenceDateEvents() const {
        return includeReferenceDateEvents_;
    }
    template <class ExtDate>
    inline std::optional<bool>& Settings<ExtDate>::includeTodaysCashFlows() {
        return includeTodaysCashFlows_;
    }
    template <class ExtDate>
    inline std::optional<bool> Settings<ExtDate>::includeTodaysCashFlows() const {
        return includeTodaysCashFlows_;
    }
    template <class ExtDate>
    inline bool& Settings<ExtDate>::enforcesTodaysHistoricFixings() {
        return enforcesTodaysHistoricFixings_;
    }
    template <class ExtDate>
    inline bool Settings<ExtDate>::enforcesTodaysHistoricFixings() const {
        return enforcesTodaysHistoricFixings_;
    }

}
#include "ql_settings.cpp"
#endif
