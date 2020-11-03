#include "all.hpp"
#include <blpapi_datetime.h>


using Datetime = BloombergLP::blpapi::Datetime;
int dayOfMonth(const Datetime&);

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

namespace QuantLib {


}

QuantLib::Actual360<Datetime> toto0;
QuantLib::Actual364<Datetime> toto1;
QuantLib::Actual365Fixed<Datetime> toto2;
QuantLib::OneDayCounter<Datetime> toto3;
QuantLib::SimpleDayCounter<Datetime> toto4;
QuantLib::Thirty360<Datetime> toto5;
QuantLib::Thirty365<Datetime> toto6;
QuantLib::ActualActual<Datetime> toto7;

#if defined(FIX_SCHEDULE_INCLUDE)

#endif