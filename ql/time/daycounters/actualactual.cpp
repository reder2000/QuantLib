#include "all.hpp"

#include <blpapi_datetime.h>
using Datetime = BloombergLP::blpapi::Datetime;

template <>
struct date_traits<Datetime> {
    using T = Datetime;
    using serial_type = int;
    using Time = double;
    using Day = int;

    static Time daysBetween(const T& d1, const T& d2) { return 0; // TODO
        }
    static serial_type onlyDaysBetween(const T&, const T&) {
        return 0; // TODO
    }
    static Day dayOfMonth(const T& d) { return d.day();
    }
    static bool isEndOfMonth(const T&) { return false;  // TODO
    }
    static T plusPeriod(const T& d, const QuantLib::Period&) {
        return d; // TODO
    }
    static QuantLib::Month month(const T& d) {
        return static_cast<QuantLib::Month>(d.month()); }
    static T& addSelf(T&d, serial_type) { return d;  // TODO
    }
    static T& addSelf(T&d, const QuantLib::Period&) {
        return d; // TODO
    }
    static QuantLib::Weekday weekday(const T& d) { return QuantLib::Sunday; // TODO
    }

    static Day dayOfYear(const T&) { return 0; //TODO
    }
    static bool isLeap(int y) { return true;  //TODO
    }
    static T& inc(T&d) { return d; //TODO
    }

    static T& dec(T& d) {
        return d; // TODO
    }
    static T add(const T&d, serial_type) { return d; //TODO
    }
    static T endOfMonth(const T& d) { return d;  //TODO
    }
};


/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

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

#include "actualactual.hpp"
#include <algorithm>

QuantLib::Actual360<Datetime> toto0;
QuantLib::Actual364<Datetime> toto1;
QuantLib::Actual365Fixed<Datetime> toto2;
QuantLib::OneDayCounter<Datetime> toto3;
QuantLib::SimpleDayCounter<Datetime> toto4;
QuantLib::Thirty360<Datetime> toto5;
QuantLib::Thirty365<Datetime> toto6;
QuantLib::ActualActual<Datetime> toto7;
QuantLib::Business252<Datetime> toto8;
