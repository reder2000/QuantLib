/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2009 StatPro Italia srl

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

#include <ql/time/calendars/weekendsonly.hpp>

namespace QuantLib {
    template <class ExtDate> inline
    WeekendsOnly<ExtDate>::WeekendsOnly() {
        // all calendar instances share the same implementation instance
        static std::shared_ptr<typename Calendar<ExtDate>::Impl> impl(new WeekendsOnly<ExtDate>::Impl);
        this->impl_ = impl;
    }
    template <class ExtDate> inline
    bool WeekendsOnly<ExtDate>::Impl::isBusinessDay(const ExtDate& date) const {
        return !isWeekend(date.weekday());
    }

}

