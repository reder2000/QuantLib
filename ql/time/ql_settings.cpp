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

#include "ql_settings.hpp"

namespace QuantLib {
    template <class ExtDate> inline
    Settings<ExtDate>::DateProxy::DateProxy()
    : ObservableValue<ExtDate>(ExtDate()) {}
    template <class ExtDate> inline
    std::ostream& operator<<(std::ostream& out,
                             const typename Settings<ExtDate>::DateProxy& p) {
        return out << ExtDate(p);
    }
    template <class ExtDate> inline
    Settings<ExtDate>::Settings()
    : includeReferenceDateEvents_(false),
      enforcesTodaysHistoricFixings_(false) {}
    template <class ExtDate> inline
    void Settings<ExtDate>::anchorEvaluationDate() {
        // set to today's date if not already set.
        if (evaluationDate_.value() == ExtDate())
            evaluationDate_ = ExtDate::todaysDate();
        // If set, no-op since the date is already anchored.
    }
    template <class ExtDate> inline
    void Settings<ExtDate>::resetEvaluationDate() {
        evaluationDate_ = ExtDate();
    }
    template <class ExtDate> inline
    SavedSettings<ExtDate>::SavedSettings()
    : evaluationDate_(Settings<ExtDate>::instance().evaluationDate()),
      includeReferenceDateEvents_(
                        Settings<ExtDate>::instance().includeReferenceDateEvents()),
      includeTodaysCashFlows_(Settings<ExtDate>::instance().includeTodaysCashFlows()),
      enforcesTodaysHistoricFixings_(
                        Settings<ExtDate>::instance().enforcesTodaysHistoricFixings()) {}
    template <class ExtDate> inline
    SavedSettings<ExtDate>::~SavedSettings() {
        try {
            if (Settings<ExtDate>::instance().evaluationDate() != evaluationDate_)
                Settings<ExtDate>::instance().evaluationDate() = evaluationDate_;
            Settings<ExtDate>::instance().includeReferenceDateEvents() =
                includeReferenceDateEvents_;
            Settings<ExtDate>::instance().includeTodaysCashFlows() =
                includeTodaysCashFlows_;
            Settings<ExtDate>::instance().enforcesTodaysHistoricFixings() =
                enforcesTodaysHistoricFixings_;
        } catch (...) {
            // nothing we can do except bailing out.
        }
    }

}
