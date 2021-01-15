/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2004, 2005, 2006, 2007 Ferdinando Ametrano
 Copyright (C) 2006 Katiuscia Manzoni
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004, 2005, 2006 StatPro Italia srl

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

/*! \file timeunit.hpp
    \brief TimeUnit enumeration
*/
#pragma once

#include "ql_qldefines.hpp"
#include "ql_types.hpp"
#include <iosfwd>

namespace QuantLib {

        //! Day number
        /*! \ingroup datetime */
        typedef Integer Day;
        //! Month names
        /*! \ingroup datetime */
        enum Month {
            January = 1,
            February = 2,
            March = 3,
            April = 4,
            May = 5,
            June = 6,
            July = 7,
            August = 8,
            September = 9,
            October = 10,
            November = 11,
            December = 12,
            Jan = 1,
            Feb = 2,
            Mar = 3,
            Apr = 4,
            Jun = 6,
            Jul = 7,
            Aug = 8,
            Sep = 9,
            Oct = 10,
            Nov = 11,
            Dec = 12
        };
        /*! \relates Month */
        std::ostream& operator<<(std::ostream&, Month);

        //! Year number
        /*! \ingroup datetime */
        typedef Integer Year;


    //! Units used to describe time periods
    /*! \ingroup datetime */
    enum TimeUnit { Days,
                    Weeks,
                    Months,
                    Years,
                    Hours,
                    Minutes,
                    Seconds,
                    Milliseconds,
					Microseconds
    };

    /*! \relates TimeUnit */
    std::ostream& operator<<(std::ostream&,
                             const TimeUnit&);

}

