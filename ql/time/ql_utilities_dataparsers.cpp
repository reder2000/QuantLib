/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2002, 2003 Decillion Pty(Ltd)
 Copyright (C) 2006 Joseph Wang
 Copyright (2) 2009 Mark Joshi
 Copyright (2) 2009 StatPro Italia srl

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

//#include <ql/utilities/dataparsers.hpp>
#include "ql_utilities_null.hpp"
#include <ql/time/period.hpp>
#include "ql_errors.hpp"
#if defined(__GNUC__) && (((__GNUC__ == 4) && (__GNUC_MINOR__ >= 8)) || (__GNUC__ > 4))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif
#if !defined(x64) && !defined(QL_PATCH_SOLARIS)
//#include <boost/lexical_cast.hpp>
#endif
#ifndef QL_PATCH_SOLARIS
//#include <boost/algorithm/string/case_conv.hpp>
//#include <boost/date_time/gregorian/gregorian.hpp>
#endif
#if defined(__GNUC__) && (((__GNUC__ == 4) && (__GNUC_MINOR__ >= 8)) || (__GNUC__ > 4))
#pragma GCC diagnostic pop
#endif
#include <locale>
#include <cctype>
#if defined(BOOST_NO_STDC_NAMESPACE)
    namespace std { using ::toupper; }
#endif

namespace QuantLib {

    namespace io {
        inline
        Integer to_integer(const std::string& str) {
        //#if !defined(x64) && !defined(QL_PATCH_SOLARIS)
        //    return boost::lexical_cast<Integer>(str.c_str());
        //#else
            return std::atoi(str.c_str());
        //#endif
        }

    }
    inline
    Period PeriodParser::parse(const std::string& str) {
        QL_REQUIRE(str.length()>1, "period string length must be at least 2");

        std::vector<std::string > subStrings;
        std::string reducedString = str;

        Size iPos, reducedStringDim = 100000, max_iter = 0;
        while (reducedStringDim>0) {
            iPos = reducedString.find_first_of("DdWwMmYy");
            Size subStringDim = iPos+1;
            reducedStringDim = reducedString.length()-subStringDim;
            subStrings.push_back(reducedString.substr(0, subStringDim));
            reducedString = reducedString.substr(iPos+1, reducedStringDim);
            ++max_iter;
            QL_REQUIRE(max_iter<str.length(), "unknown '{}' unit",str);
        }

        Period result = parseOnePeriod(subStrings[0]);
        for (Size i=1; i<subStrings.size(); ++i)
            result += parseOnePeriod(subStrings[i]);
        return result;
    }
    inline
    Period PeriodParser::parseOnePeriod(const std::string& str) {
        QL_REQUIRE(str.length()>1, "single period require a string of at "
                   "least 2 characters");

        Size iPos = str.find_first_of("DdWwMmYy");
        QL_REQUIRE(iPos==str.length()-1, "unknown '{}' unit",
                   str.substr(str.length()-1, str.length()) );
        TimeUnit units = Days;
        char abbr = static_cast<char>(std::toupper(str[iPos]));
        if      (abbr == 'D') units = Days;
        else if (abbr == 'W') units = Weeks;
        else if (abbr == 'M') units = Months;
        else if (abbr == 'Y') units = Years;

        Size nPos = str.find_first_of("-+0123456789");
        QL_REQUIRE(nPos<iPos, "no numbers of {} provided",units);
        Integer n;
        try {
            n = io::to_integer(str.substr(nPos,iPos));
        } catch (std::exception& e) {
            QL_FAIL("unable to parse the number of units of {} in '{}'. Error:", units,str, e.what());
        }

        return Period(n, units);
    }
    template <class ExtDate> inline
    ExtDate DateParser<ExtDate>::parseFormatted(const std::string& str,
                                    const std::string& fmt) {
        //#ifndef QL_PATCH_SOLARIS
        //using namespace boost::gregorian;

        //date boostDate;
        //std::istringstream is(str);
        //is.imbue(std::locale(std::locale(), new date_input_facet(fmt)));
        //is >> boostDate;
        //date_duration noDays = boostDate - date(1901, 1, 1);
        //return DateAdaptor<ExtDate>::Date(1, January, 1901) + noDays.days();
        //#else
        QL_FAIL("DateParser::parseFormatted not supported (boost removed)");
        //#endif
    }
    template <class ExtDate> inline
    ExtDate DateParser<ExtDate>::parseISO(const std::string& str) {
        QL_REQUIRE(str.size() == 10 && str[4] == '-' && str[7] == '-',
                   "invalid format");
        Integer year = io::to_integer(str.substr(0, 4));
        Month month = static_cast<Month>(io::to_integer(str.substr(5, 2)));
        Integer day = io::to_integer(str.substr(8, 2));

        return DateAdaptor<ExtDate>::Date(day, month, year);
    }

}
