/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2009 Ferdinando Ametrano

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

/*! \file ecb.hpp
    \brief European Central Bank reserve maintenance date functions
*/
#pragma once
#ifndef quantlib_ecb_hpp
#define quantlib_ecb_hpp

#include <ql/time/date.hpp>
#include <set>
#include <vector>

namespace QuantLib {
    template <class ExtDate>
    using setExtDate = std::set<ExtDate, Less<ExtDate> >;

    //! European Central Bank reserve maintenance dates
    template <class ExtDate>
    struct ECB {
        static const setExtDate<ExtDate>& knownDates();
        static void addDate(const ExtDate& d);
        static void removeDate(const ExtDate& d);

        //! maintenance period start date in the given month/year
        static ExtDate date(Month m,
                         Year y) { return nextDate(ExtDate(1, m, y) - 1); }

        /*! returns the ECB date for the given ECB code
            (e.g. March xxth, 2013 for MAR10).

            \warning It raises an exception if the input
                     string is not an ECB code
        */
        static ExtDate date(const std::string& ecbCode,
                         const ExtDate& referenceDate = ExtDate());

        /*! returns the ECB code for the given date
            (e.g. MAR10 for March xxth, 2010).

            \warning It raises an exception if the input
                     date is not an ECB date
        */
        static std::string code(const ExtDate& ecbDate);

        //! next maintenance period start date following the given date
        static ExtDate nextDate(const ExtDate& d = ExtDate());

        //! next maintenance period start date following the given ECB code
        static ExtDate nextDate(const std::string& ecbCode,
                             const ExtDate& referenceDate = ExtDate()) {
            return nextDate(date(ecbCode, referenceDate));
        }

        //! next maintenance period start dates following the given date
        static std::vector<ExtDate> nextDates(const ExtDate& d = ExtDate());

        //! next maintenance period start dates following the given code
        static std::vector<ExtDate> nextDates(const std::string& ecbCode,
                                           const ExtDate& referenceDate = ExtDate()) {
            return nextDates(date(ecbCode, referenceDate));
        }

        /*! returns whether or not the given date is
            a maintenance period start date */
        static bool isECBdate(const ExtDate& d) {
            ExtDate date = nextDate(to_DateLike<ExtDate>(d)-1);
            return to_DateLike<ExtDate>(d)==to_DateLike<ExtDate>(date);
        }

        //! returns whether or not the given string is an ECB code
        static bool isECBcode(const std::string& in);

        //! next ECB code following the given date
        static std::string nextCode(const ExtDate& d = ExtDate()) {
            return code(nextDate(d));
        }

        //! next ECB code following the given code
        static std::string nextCode(const std::string& ecbCode);

    };

}
#include "ecb.cpp"
#endif
