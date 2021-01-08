/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004, 2005, 2006, 2007 StatPro Italia srl
 Copyright (C) 2004, 2005, 2006 Ferdinando Ametrano
 Copyright (C) 2006 Katiuscia Manzoni
 Copyright (C) 2006 Toyin Akin
 Copyright (C) 2015 Klaus Spanderen
 Copyright (C) 2020 Leonardo Arcari
 Copyright (C) 2020 Kline s.r.l.

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

#include <ql/time/date.hpp>
#include "ql_utilities_dataformatters.hpp"
#include "ql_errors.hpp"
#if defined(__GNUC__) && (((__GNUC__ == 4) && (__GNUC_MINOR__ >= 8)) || (__GNUC__ > 4))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif
//#include <boost/date_time/gregorian/gregorian.hpp>
//#include <boost/date_time/posix_time/posix_time_types.hpp>
//#include <boost/functional/hash.hpp>
#if defined(__GNUC__) && (((__GNUC__ == 4) && (__GNUC_MINOR__ >= 8)) || (__GNUC__ > 4))
#pragma GCC diagnostic pop
#endif
#include <iomanip>
#include <ctime>
#include <compare>

#if defined(BOOST_NO_STDC_NAMESPACE)
    namespace std { using ::time; using ::time_t; using ::tm;
                    using ::gmtime; using ::localtime; }
#endif

#ifdef QL_HIGH_RESOLUTION_DATE
using boost::posix_time::ptime;
using boost::posix_time::time_duration;
#endif


namespace QuantLib {
#ifndef QL_HIGH_RESOLUTION_DATE
    // constructors
    //DateLike<ExtDate>::Date()
    //: serialNumber_(typename DateLike<ExtDate>::serial_type(0)) {}

    //DateLike<ExtDate>::Date(typename DateLike<ExtDate>::serial_type serialNumber)
    //: serialNumber_(serialNumber) {
    //    checkSerialNumber(serialNumber);
    //}

    //DateLike<ExtDate>::Date(Day d, Month m, Year y) {
    //    QL_REQUIRE(y > 1900 && y < 2200,
    //               "year " << y << " out of bound. It must be in [1901,2199]");
    //    QL_REQUIRE(Integer(m) > 0 && Integer(m) < 13,
    //               "month " << Integer(m)
    //               << " outside January-December range [1,12]");

    //    bool leap = isLeap(y);
    //    Day len = monthLength(m,leap), offset = monthOffset(m,leap);
    //    QL_REQUIRE(d <= len && d > 0,
    //               "day outside month (" << Integer(m) << ") day-range "
    //               << "[1," << len << "]");

    //    serialNumber_ = d + offset + yearOffset(y);
    //}
    template <class ExtDate>
    inline Month DateLike<ExtDate>::month(serial_type s) const {
        Day d = dayOfYear(s); // dayOfYear is 1 based
        Integer m = d / 30 + 1;
        bool leap = isLeap(year(s));
        while (d <= monthOffset(Month(m), leap))
            --m;
        while (d > monthOffset(Month(m + 1), leap)) // NOLINT(misc-misplaced-widening-cast)
            ++m;
        return Month(m);
    }
    template <class ExtDate>
    inline
    Month DateLike<ExtDate>::month() const {
        auto s = serialNumber();
        return month(s);
    }
    template <class ExtDate>
    inline Year DateLike<ExtDate>::year(std::int_fast32_t serialNumber_) const {
        Year y = (serialNumber_ / 365) + 1900;
        // yearOffset(y) is December 31st of the preceding year
        if (serialNumber_ <= yearOffset(y))
            --y;
        return y;
    }
    template <class ExtDate>
    inline
    Year DateLike<ExtDate>::year() const {
        auto s = serialNumber();
        return year(s);
    }
    template <class ExtDate>
    inline YearMonth DateLike<ExtDate> :: year_month() const {
        auto s = serialNumber();
        return {year(s), month(s)};
    }
    template <class ExtDate>
    inline
    DateLike<ExtDate>& DateLike<ExtDate>::operator+=(typename DateLike<ExtDate>::serial_type days) {
        serial_type serial = serialNumber() + days;
        checkSerialNumber(serial);
        *static_cast<ExtDate*>(this) = DateAdaptor<ExtDate>::Date(serial);
        return *this;
    }
    template <class ExtDate>    inline
    DateLike<ExtDate>& DateLike<ExtDate>::operator+=(const Period& p) {
        auto serial = advance(*this,p.length(),p.units()).serialNumber();
        *static_cast<ExtDate*>(this) = DateAdaptor<ExtDate>::Date(serial);
        return *this;
    }
    template <class ExtDate>    inline
    DateLike<ExtDate>& DateLike<ExtDate>::operator-=(typename DateLike<ExtDate>::serial_type days) {
        typename DateLike<ExtDate>::serial_type serial = serialNumber() - days;
        checkSerialNumber(serial);
        serialNumber() = serial;
        return *this;
    }
    template <class ExtDate>    inline
    DateLike<ExtDate>& DateLike<ExtDate>::operator-=(const Period& p) {
        auto serial = advance(*this,-p.length(),p.units()).serialNumber();
        *static_cast<ExtDate*>(this) = DateAdaptor<ExtDate>::Date(serial);
        return *this;
    }
    template <class ExtDate>    inline
    DateLike<ExtDate>& DateLike<ExtDate>::operator++() {
        typename DateLike<ExtDate>::serial_type serial = serialNumber() + 1;
        checkSerialNumber(serial);
        *static_cast<ExtDate*>(this) = DateAdaptor<ExtDate>::Date(serial);
        return *this;
    }
    template <class ExtDate>    inline
    DateLike<ExtDate>& DateLike<ExtDate>::operator--() {
        typename DateLike<ExtDate>::serial_type serial = serialNumber() - 1;
        checkSerialNumber(serial);
        *static_cast<ExtDate*>(this) = DateAdaptor<ExtDate>::Date(serial);
        return *this;
    }
    template <class ExtDate>    inline
    DateLike<ExtDate> DateLike<ExtDate>::advance(const DateLike<ExtDate>& date, Integer n, TimeUnit units) {
        switch (units) {
          case Days:
            return date + n;
          case Weeks:
            return date + 7*n;
          case Months: {
            auto s = date.serialNumber();
            Day d = date.dayOfMonth(s);
            Integer m = Integer(date.month(s))+n;
            Year y = date.year(s);
            while (m > 12) {
                m -= 12;
                y += 1;
            }
            while (m < 1) {
                m += 12;
                y -= 1;
            }

            QL_ENSURE(y >= 1900 && y <= 2199,
                      "year {} out of bounds. It must be in [1901,2199]",y);

            Integer length = monthLength(Month(m), isLeap(y));
            if (d > length)
                d = length;

            auto res = DateAdaptor<ExtDate>::Date(d, Month(m), y);
            return static_cast<DateLike<ExtDate> >(res);
          }
          case Years: {
              auto s = date.serialNumber();
              Day d = date.dayOfMonth(s);
              Month m = date.month(s);
              Year y = date.year(s)+n;

            QL_ENSURE(y >= 1900 && y <= 2199,
                      "year {} out of bounds. It must be in [1901,2199]",y);

              if (d == 29 && m == February && !isLeap(y))
                  d = 28;

              return static_cast<DateLike<ExtDate> >(DateAdaptor<ExtDate>::Date(d, m, y));
          }
          default:
            QL_FAIL("undefined time units");
        }
    }
    template <class ExtDate>    inline
    bool DateLike<ExtDate>::isLeap(Year y) {
        static const bool YearIsLeap[] = {
            // 1900 is leap in agreement with Excel's bug
            // 1900 is out of valid date range anyway
            // 1900-1909
             true,false,false,false, true,false,false,false, true,false,
            // 1910-1919
            false,false, true,false,false,false, true,false,false,false,
            // 1920-1929
             true,false,false,false, true,false,false,false, true,false,
            // 1930-1939
            false,false, true,false,false,false, true,false,false,false,
            // 1940-1949
             true,false,false,false, true,false,false,false, true,false,
            // 1950-1959
            false,false, true,false,false,false, true,false,false,false,
            // 1960-1969
             true,false,false,false, true,false,false,false, true,false,
            // 1970-1979
            false,false, true,false,false,false, true,false,false,false,
            // 1980-1989
             true,false,false,false, true,false,false,false, true,false,
            // 1990-1999
            false,false, true,false,false,false, true,false,false,false,
            // 2000-2009
             true,false,false,false, true,false,false,false, true,false,
            // 2010-2019
            false,false, true,false,false,false, true,false,false,false,
            // 2020-2029
             true,false,false,false, true,false,false,false, true,false,
            // 2030-2039
            false,false, true,false,false,false, true,false,false,false,
            // 2040-2049
             true,false,false,false, true,false,false,false, true,false,
            // 2050-2059
            false,false, true,false,false,false, true,false,false,false,
            // 2060-2069
             true,false,false,false, true,false,false,false, true,false,
            // 2070-2079
            false,false, true,false,false,false, true,false,false,false,
            // 2080-2089
             true,false,false,false, true,false,false,false, true,false,
            // 2090-2099
            false,false, true,false,false,false, true,false,false,false,
            // 2100-2109
            false,false,false,false, true,false,false,false, true,false,
            // 2110-2119
            false,false, true,false,false,false, true,false,false,false,
            // 2120-2129
             true,false,false,false, true,false,false,false, true,false,
            // 2130-2139
            false,false, true,false,false,false, true,false,false,false,
            // 2140-2149
             true,false,false,false, true,false,false,false, true,false,
            // 2150-2159
            false,false, true,false,false,false, true,false,false,false,
            // 2160-2169
             true,false,false,false, true,false,false,false, true,false,
            // 2170-2179
            false,false, true,false,false,false, true,false,false,false,
            // 2180-2189
             true,false,false,false, true,false,false,false, true,false,
            // 2190-2199
            false,false, true,false,false,false, true,false,false,false,
            // 2200
            false
        };
        QL_REQUIRE(y>=1900 && y<=2200, "year outside valid range");
        return YearIsLeap[y-1900];
    }

    template <class ExtDate>    inline
    Integer DateLike<ExtDate>::monthLength(Month m, bool leapYear) {
        static const Integer MonthLength[] = {
            31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
        };
        static const Integer MonthLeapLength[] = {
            31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
        };
        return (leapYear? MonthLeapLength[m-1] : MonthLength[m-1]);
    }
    template <class ExtDate>    inline
    Integer DateLike<ExtDate>::monthOffset(Month m, bool leapYear) {
        static const Integer MonthOffset[] = {
              0,  31,  59,  90, 120, 151,   // Jan - Jun
            181, 212, 243, 273, 304, 334,   // Jun - Dec
            365     // used in dayOfMonth to bracket day
        };
        static const Integer MonthLeapOffset[] = {
              0,  31,  60,  91, 121, 152,   // Jan - Jun
            182, 213, 244, 274, 305, 335,   // Jun - Dec
            366     // used in dayOfMonth to bracket day
        };
        return (leapYear? MonthLeapOffset[m-1] : MonthOffset[m-1]);
    }
    template <class ExtDate>    inline
    typename DateLike<ExtDate>::serial_type DateLike<ExtDate>::yearOffset(Year y) {
        // the list of all December 31st in the preceding year
        // e.g. for 1901 yearOffset[1] is 366, that is, December 31 1900
        static const typename DateLike<ExtDate>::serial_type YearOffset[] = {
            // 1900-1909
                0,  366,  731, 1096, 1461, 1827, 2192, 2557, 2922, 3288,
            // 1910-1919
             3653, 4018, 4383, 4749, 5114, 5479, 5844, 6210, 6575, 6940,
            // 1920-1929
             7305, 7671, 8036, 8401, 8766, 9132, 9497, 9862,10227,10593,
            // 1930-1939
            10958,11323,11688,12054,12419,12784,13149,13515,13880,14245,
            // 1940-1949
            14610,14976,15341,15706,16071,16437,16802,17167,17532,17898,
            // 1950-1959
            18263,18628,18993,19359,19724,20089,20454,20820,21185,21550,
            // 1960-1969
            21915,22281,22646,23011,23376,23742,24107,24472,24837,25203,
            // 1970-1979
            25568,25933,26298,26664,27029,27394,27759,28125,28490,28855,
            // 1980-1989
            29220,29586,29951,30316,30681,31047,31412,31777,32142,32508,
            // 1990-1999
            32873,33238,33603,33969,34334,34699,35064,35430,35795,36160,
            // 2000-2009
            36525,36891,37256,37621,37986,38352,38717,39082,39447,39813,
            // 2010-2019
            40178,40543,40908,41274,41639,42004,42369,42735,43100,43465,
            // 2020-2029
            43830,44196,44561,44926,45291,45657,46022,46387,46752,47118,
            // 2030-2039
            47483,47848,48213,48579,48944,49309,49674,50040,50405,50770,
            // 2040-2049
            51135,51501,51866,52231,52596,52962,53327,53692,54057,54423,
            // 2050-2059
            54788,55153,55518,55884,56249,56614,56979,57345,57710,58075,
            // 2060-2069
            58440,58806,59171,59536,59901,60267,60632,60997,61362,61728,
            // 2070-2079
            62093,62458,62823,63189,63554,63919,64284,64650,65015,65380,
            // 2080-2089
            65745,66111,66476,66841,67206,67572,67937,68302,68667,69033,
            // 2090-2099
            69398,69763,70128,70494,70859,71224,71589,71955,72320,72685,
            // 2100-2109
            73050,73415,73780,74145,74510,74876,75241,75606,75971,76337,
            // 2110-2119
            76702,77067,77432,77798,78163,78528,78893,79259,79624,79989,
            // 2120-2129
            80354,80720,81085,81450,81815,82181,82546,82911,83276,83642,
            // 2130-2139
            84007,84372,84737,85103,85468,85833,86198,86564,86929,87294,
            // 2140-2149
            87659,88025,88390,88755,89120,89486,89851,90216,90581,90947,
            // 2150-2159
            91312,91677,92042,92408,92773,93138,93503,93869,94234,94599,
            // 2160-2169
            94964,95330,95695,96060,96425,96791,97156,97521,97886,98252,
            // 2170-2179
            98617,98982,99347,99713,100078,100443,100808,101174,101539,101904,
            // 2180-2189
            102269,102635,103000,103365,103730,104096,104461,104826,105191,105557,
            // 2190-2199
            105922,106287,106652,107018,107383,107748,108113,108479,108844,109209,
            // 2200
            109574
        };
        return YearOffset[y-1900];
    }

#else

    namespace {
        const boost::gregorian::DateLike<ExtDate>& serialNumberDateReference() {
            static const boost::gregorian::date dateReference(
                1899, boost::gregorian::Dec, 30);
            return dateReference;
        }


#define compatibleEnums (   int(boost::date_time::Monday)   +1 == Monday    \
                         && int(boost::date_time::Tuesday)  +1 == Tuesday   \
                         && int(boost::date_time::Wednesday)+1 == Wednesday \
                         && int(boost::date_time::Thursday) +1 == Thursday  \
                         && int(boost::date_time::Friday)   +1 == Friday    \
                         && int(boost::date_time::Saturday) +1 == Saturday  \
                         && int(boost::date_time::Sunday)   +1 == Sunday    \
                         && int(boost::date_time::Jan) == January           \
                         && int(boost::date_time::Feb) == February          \
                         && int(boost::date_time::Mar) == March             \
                         && int(boost::date_time::Apr) == April             \
                         && int(boost::date_time::May) == May               \
                         && int(boost::date_time::Jun) == June              \
                         && int(boost::date_time::Jul) == July              \
                         && int(boost::date_time::Aug) == August            \
                         && int(boost::date_time::Sep) == September         \
                         && int(boost::date_time::Oct) == October           \
                         && int(boost::date_time::Nov) == November          \
                         && int(boost::date_time::Dec) == December           )

        template <bool compatible>
        Weekday mapBoostDateType2QL(boost::gregorian::greg_weekday d) {
            if (compatible) {
                return Weekday(d.as_number() + 1);
            }
            else {
                switch (d) {
                  case boost::date_time::Monday   : return Monday;
                  case boost::date_time::Tuesday  : return Tuesday;
                  case boost::date_time::Wednesday: return Wednesday;
                  case boost::date_time::Thursday : return Thursday;
                  case boost::date_time::Friday   : return Friday;
                  case boost::date_time::Saturday : return Saturday;
                  case boost::date_time::Sunday   : return Sunday;
                  default:
                    QL_FAIL("unknown boost date_time day of week given");
                }
            }
        }

        template <bool compatible>
        Month mapBoostDateType2QL(boost::gregorian::greg_month m) {
            if (compatible) {
                return Month(m.as_number());
            }
            else {
                switch (m) {
                  case boost::date_time::Jan : return January;
                  case boost::date_time::Feb : return February;
                  case boost::date_time::Mar : return March;
                  case boost::date_time::Apr : return April;
                  case boost::date_time::May : return May;
                  case boost::date_time::Jun : return June;
                  case boost::date_time::Jul : return July;
                  case boost::date_time::Aug : return August;
                  case boost::date_time::Sep : return September;
                  case boost::date_time::Oct : return October;
                  case boost::date_time::Nov : return November;
                  case boost::date_time::Dec : return December;
                  default:
                    QL_FAIL("unknown boost date_time month of week given");
                }
            }
        }


        template <bool compatible>
        boost::gregorian::greg_month mapQLDateType2Boost(Month m) {
            if (compatible) {
                return boost::gregorian::greg_month(m);
            }
            else {
                switch (m) {
                  case January  : return boost::date_time::Jan;
                  case February : return boost::date_time::Feb;
                  case March    : return boost::date_time::Mar;
                  case April    : return boost::date_time::Apr;
                  case May      : return boost::date_time::May;
                  case June     : return boost::date_time::Jun;
                  case July     : return boost::date_time::Jul;
                  case August   : return boost::date_time::Aug;
                  case September: return boost::date_time::Sep;
                  case October  : return boost::date_time::Oct;
                  case November : return boost::date_time::Nov;
                  case December : return boost::date_time::Dec;
                  default:
                    QL_FAIL("unknown boost date_time month of week given");
                }
            }
        }

        void advance(ptime& dt, Integer n, TimeUnit units) {
            using boost::gregorian::gregorian_calendar;

            switch (units) {
              case Days:
                dt += boost::gregorian::days(n);
                break;
              case Weeks:
                dt += boost::gregorian::weeks(n);
                break;
              case Months:
              case Years : {
                  const boost::gregorian::date date = dt.date();
                  const Day eoM = gregorian_calendar::end_of_month_day(
                      date.year(), date.month());

                  if (units == Months) {
                      dt += boost::gregorian::months(n);
                  }
                  else {
                      dt += boost::gregorian::years(n);
                  }

                  if (date.day() == eoM) {
                      // avoid snap-to-end-of-month
                      // behavior of boost::date_time
                      const Day newEoM
                            = gregorian_calendar::end_of_month_day(
                                  dt.date().year(), dt.date().month());

                      if (newEoM > eoM) {
                          dt -= boost::gregorian::days(newEoM - eoM);
                      }
                  }
                 }
                break;
              default:
                QL_FAIL("undefined time units");
           }
        }

        boost::gregorian::date gregorianDate(Year y, Month m, Day d) {
            QL_REQUIRE(y > 1900 && y < 2200,
                       "year " << y << " out of bound. It must be in [1901,2199]");
            QL_REQUIRE(Integer(m) > 0 && Integer(m) < 13,
                       "month " << Integer(m)
                       << " outside January-December range [1,12]");

            const boost::gregorian::greg_month bM
                = mapQLDateType2Boost<compatibleEnums>(m);

            const Day len =
                boost::gregorian::gregorian_calendar::end_of_month_day(y, bM);
            QL_REQUIRE(d <= len && d > 0,
                       "day outside month (" << Integer(m) << ") day-range "
                       << "[1," << len << "]");

            return boost::gregorian::date(y, bM, d);
        }
    }


    DateLike<ExtDate>::Date()
    : dateTime_(serialNumberDateReference()) {}

    DateLike<ExtDate>::Date(const ptime& dateTime)
    : dateTime_(dateTime) {}

    DateLike<ExtDate>::Date(Day d, Month m, Year y)
    : dateTime_(gregorianDate(y, m, d)) {}

    DateLike<ExtDate>::Date(Day d, Month m, Year y,
               Hour hours, Minute minutes, Second seconds,
               Millisecond millisec, Microsecond microsec)
    : dateTime_(
         gregorianDate(y, m, d),
         boost::posix_time::time_duration(
             hours, minutes, seconds,
               millisec*(time_duration::ticks_per_second()/1000)
             + microsec*(time_duration::ticks_per_second()/1000000))) {}

    DateLike<ExtDate>::Date(typename typename DateLike<ExtDate>::serial_type serialNumber)
    : dateTime_(
         serialNumberDateReference() +
         boost::gregorian::days(serialNumber)) {
        checkSerialNumber(serialNumber);
    }

    Weekday DateLike<ExtDate>::weekday() const {
        return mapBoostDateType2QL<compatibleEnums>(
            dateTime_.date().day_of_week());
    }

    Day DateLike<ExtDate>::dayOfMonth() const {
        return dateTime_.date().day();
    }

    Day DateLike<ExtDate>::dayOfYear() const {
        return dateTime_.date().day_of_year();
    }

    Month DateLike<ExtDate>::month() const {
        return mapBoostDateType2QL<compatibleEnums>(dateTime_.date().month());
    }

    Year DateLike<ExtDate>::year() const {
        return dateTime_.date().year();
    }

    Hour DateLike<ExtDate>::hours() const {
        return dateTime_.time_of_day().hours();
    }

    Minute DateLike<ExtDate>::minutes() const {
        return dateTime_.time_of_day().minutes();
    }

    Second DateLike<ExtDate>::seconds() const {
        return dateTime_.time_of_day().seconds();
    }

    Time DateLike<ExtDate>::fractionOfDay() const {
        const time_duration t = dateTime().time_of_day();

        const Time seconds
            = (t.hours()*60.0 + t.minutes())*60.0 + t.seconds()
                + Real(t.fractional_seconds())/ticksPerSecond();

        return seconds/86400.0; // ignore any DST hocus-pocus
    }

    Time DateLike<ExtDate>::fractionOfSecond() const {
        return dateTime_.time_of_day().fractional_seconds()
            /Real(ticksPerSecond());
    }

    Millisecond DateLike<ExtDate>::milliseconds() const {
        return dateTime_.time_of_day().fractional_seconds()
               /(ticksPerSecond()/1000);
    }

    Microsecond DateLike<ExtDate>::microseconds() const {
        return (dateTime_.time_of_day().fractional_seconds()
                - milliseconds()*(time_duration::ticks_per_second()/1000))
            /(ticksPerSecond()/1000000);
    }

    time_duration::tick_type DateLike<ExtDate>::ticksPerSecond() {
        return time_duration::ticks_per_second();
    }

    typename typename DateLike<ExtDate>::serial_type DateLike<ExtDate>::serialNumber() const {
        const typename typename DateLike<ExtDate>::serial_type n = (dateTime_.date()
            - serialNumberDateReference()).days();
        checkSerialNumber(n);

        return n;
    }

    const ptime& DateLike<ExtDate>::dateTime() const { return dateTime_; }

    DateLike<ExtDate>& DateLike<ExtDate>::operator+=(typename DateLike<ExtDate>::serial_type d) {
        dateTime_ += boost::gregorian::days(d);
        return *this;
    }

    DateLike<ExtDate>& DateLike<ExtDate>::operator+=(const Period& p) {
        advance(dateTime_, p.length(), p.units());
        return *this;
    }

    DateLike<ExtDate>& DateLike<ExtDate>::operator-=(typename DateLike<ExtDate>::serial_type d) {
        dateTime_ -= boost::gregorian::days(d);
        return *this;
    }
    DateLike<ExtDate>& DateLike<ExtDate>::operator-=(const Period& p) {
        advance(dateTime_, -p.length(), p.units());
        return *this;
    }

    DateLike<ExtDate>& DateLike<ExtDate>::operator++() {
        dateTime_ +=boost::gregorian::days(1);
        return *this;
    }

    DateLike<ExtDate>& DateLike<ExtDate>::operator--() {
        dateTime_ -=boost::gregorian::days(1);
        return *this;
    }

    DateLike<ExtDate> DateLike<ExtDate>::operator+(typename DateLike<ExtDate>::serial_type days) const {
        DateLike<ExtDate> retVal(*this);
        retVal+=days;

        return retVal;
    }

    DateLike<ExtDate> DateLike<ExtDate>::operator-(typename DateLike<ExtDate>::serial_type days) const {
        DateLike<ExtDate> retVal(*this);
        retVal-=days;

        return retVal;
    }

    DateLike<ExtDate> DateLike<ExtDate>::operator+(const Period& p) const {
        DateLike<ExtDate> retVal(*this);
        retVal+=p;

        return retVal;
    }

    DateLike<ExtDate> DateLike<ExtDate>::operator-(const Period& p) const {
        DateLike<ExtDate> retVal(*this);
        retVal-=p;

        return retVal;
    }

    DateLike<ExtDate> DateLike<ExtDate>::localDateTime() {
        return Date(boost::posix_time::microsec_clock::local_time());
    }

    DateLike<ExtDate> DateLike<ExtDate>::universalDateTime() {
        return Date(boost::posix_time::microsec_clock::universal_time());
    }

    bool DateLike<ExtDate>::isLeap(Year y) {
        return boost::gregorian::gregorian_calendar::is_leap_year(y);
    }

    DateLike<ExtDate> DateLike<ExtDate>::endOfMonth(const DateLike<ExtDate>& d) {
        const Month m = d.month();
        const Year y = d.year();
        const Day eoM = boost::gregorian::gregorian_calendar::end_of_month_day(
            d.year(), mapQLDateType2Boost<compatibleEnums>(d.month()));

        return Date(eoM, m, y);
    }

    bool DateLike<ExtDate>::isEndOfMonth(const DateLike<ExtDate>& d) {
        return d.dayOfMonth() ==
            boost::gregorian::gregorian_calendar::end_of_month_day(
                d.year(), mapQLDateType2Boost<compatibleEnums>(d.month()));
    }


    typename DateLike<ExtDate>::serial_type operator-(const DateLike<ExtDate>& d1, const DateLike<ExtDate>& d2) {
        return (d1.dateTime().date() - d2.dateTime().date()).days();
    }

    Time daysBetween(const DateLike<ExtDate>& d1, const DateLike<ExtDate>& d2) {
        const typename DateLike<ExtDate>::serial_type days = d2 - d1;
        return days + d2.fractionOfDay() - d1.fractionOfDay();
    }

    bool operator<(const DateLike<ExtDate>& d1, const DateLike<ExtDate>& d2) {
        return (d1.dateTime() < d2.dateTime());
    }

    bool operator<=(const DateLike<ExtDate>& d1, const DateLike<ExtDate>& d2) {
        return (d1.dateTime() <= d2.dateTime());
    }

    bool operator>(const DateLike<ExtDate>& d1, const DateLike<ExtDate>& d2) {
        return (d1.dateTime() > d2.dateTime());
    }

    bool operator>=(const DateLike<ExtDate>& d1, const DateLike<ExtDate>& d2) {
        return (d1.dateTime() >= d2.dateTime());
    }

    bool operator==(const DateLike<ExtDate>& d1, const DateLike<ExtDate>& d2) {
        return (d1.dateTime() == d2.dateTime());
    }

    bool operator!=(const DateLike<ExtDate>& d1, const DateLike<ExtDate>& d2) {
        return (d1.dateTime() != d2.dateTime());
    }
#endif
    template <class ExtDate>    inline
    typename DateLike<ExtDate>::serial_type DateLike<ExtDate>::minimumSerialNumber() {
        return 367;       // Jan 1st, 1901
    }
    template <class ExtDate>    inline
    typename DateLike<ExtDate>::serial_type DateLike<ExtDate>::maximumSerialNumber() {
        return 109574;    // Dec 31st, 2199
    }
    template <class ExtDate>    inline
    void DateLike<ExtDate>::checkSerialNumber(typename DateLike<ExtDate>::serial_type serialNumber) {
        QL_REQUIRE(serialNumber >= minimumSerialNumber() && serialNumber <= maximumSerialNumber(),
                   "Date's serial number ( {} ) outside "
                   "allowed range [{}-{}], i.e. [{}{}-{}]",
                   serialNumber, minimumSerialNumber(), maximumSerialNumber(), minDate().asExtDate(), maxDate().asExtDate());
                              
    }
    template <class ExtDate>    inline
    DateLike<ExtDate> DateLike<ExtDate>::minDate() {
        static const DateLike<ExtDate> minimumDate{ DateAdaptor<ExtDate>::Date(minimumSerialNumber()) };
        return minimumDate;
    }
    template <class ExtDate>    inline
    DateLike<ExtDate> DateLike<ExtDate>::maxDate() {
        static const DateLike<ExtDate> maximumDate{ DateAdaptor<ExtDate>::Date(maximumSerialNumber()) };
        return maximumDate;
    }
    template <class ExtDate>    inline
    DateLike<ExtDate> DateLike<ExtDate>::operator++(int ) {
        DateLike<ExtDate> old(*this);
        ++*this; // use the pre-increment
        return old;
    }
    template <class ExtDate>    inline
    DateLike<ExtDate> DateLike<ExtDate>::operator--(int ) {
        DateLike<ExtDate> old(*this);
        --*this; // use the pre-decrement
        return old;
    }
    template <class ExtDate>    inline
    DateLike<ExtDate> DateLike<ExtDate>::todaysDate() {
        std::time_t t;

        if (std::time(&t) == std::time_t(-1)) // -1 means time() didn't work
            return DateLike<ExtDate>();
        std::tm *lt = std::localtime(&t);
        ExtDate res(DateAdaptor<ExtDate>::Date(Day(lt->tm_mday),
                        Month(lt->tm_mon+1),
                        Year(lt->tm_year+1900)));
        return *static_cast<T*>(&res);
    }
    template <class ExtDate>    inline
    DateLike<ExtDate> DateLike<ExtDate>::nextWeekday(const DateLike<ExtDate>& d, Weekday dayOfWeek) {
        Weekday wd = d.weekday();
        return d + ((wd>dayOfWeek ? 7 : 0) - wd + dayOfWeek);
    }
    template <class ExtDate>    inline
    DateLike<ExtDate> DateLike<ExtDate>::nthWeekday(Size nth, Weekday dayOfWeek,
                          Month m, Year y) {
        QL_REQUIRE(nth>0,
                   "zeroth day of week in a given (month, year) is undefined");
        QL_REQUIRE(nth<6,
                   "no more than 5 weekday in a given (month, year)");
        //Weekday first = static_cast<DateLike<ExtDate>>(DateAdaptor<ExtDate>::Date(1, m, y)).weekday();
        Weekday first =
            QuantLib::Date(1, m, y).weekday();
        Size skip = nth - (dayOfWeek>=first ? 1 : 0);
        auto res = DateAdaptor<ExtDate>::Date((1 + dayOfWeek + skip*7) - first, m, y);
        return static_cast<DateLike<ExtDate> >(res);
    }
#if defined(QL_SETTINGS_ARE_REMOVED)
    // month formatting
    inline
    std::ostream& operator<<(std::ostream& out, Month m) {
        switch (m) {
          case January:
            return out << "January";
          case February:
            return out << "February";
          case March:
            return out << "March";
          case April:
            return out << "April";
          case May:
            return out << "May";
          case June:
            return out << "June";
          case July:
            return out << "July";
          case August:
            return out << "August";
          case September:
            return out << "September";
          case October:
            return out << "October";
          case November:
            return out << "November";
          case December:
            return out << "December";
          default:
            QL_FAIL("unknown month ({})" , Integer(m));
        }
    }
#endif //defined(QL_SETTINGS_ARE_REMOVED)
    template <class ExtDate>    inline
    std::size_t hash_value(const DateLike<ExtDate>& d) {
#ifdef QL_HIGH_RESOLUTION_DATE
        std::size_t seed = 0;
        boost::hash_combine(seed, d.serialNumber());
        boost::hash_combine(seed, d.dateTime().time_of_day().total_nanoseconds());
        return seed;
#else

        return std::hash<typename DateLike<ExtDate>::serial_type>()(d.serialNumber());
#endif
    }

    // date formatting
    template <class ExtDate>    inline
    std::ostream& operator<<(std::ostream& out, const DateLike<ExtDate>& d) {
        return out << io::long_date(Date(d.serialNumber()));
    }

    namespace ext_detail {

        struct FormatResetter {
            // An instance of this object will have undefined behaviour
            // if the object out passed in the constructor is destroyed
            // before this instance
            struct nopunct : std::numpunct<char> {
                std::string do_grouping() const {return "";}
            };
            explicit FormatResetter(std::ostream &out)
                : out_(&out), flags_(out.flags()), filler_(out.fill()),
                  loc_(out.getloc()) {
                std::locale loc (out.getloc(),new nopunct);
                out.imbue(loc);
                out << std::resetiosflags(
                    std::ios_base::adjustfield | std::ios_base::basefield |
                    std::ios_base::floatfield | std::ios_base::showbase |
                    std::ios_base::showpos | std::ios_base::uppercase);
                out << std::right;
            }
            ~FormatResetter() {
                out_->flags(flags_);
                out_->fill(filler_);
                out_->imbue(loc_);
            }
            std::ostream *out_;
            std::ios_base::fmtflags flags_;
            char filler_;
            std::locale loc_;
        };
        template <class ExtDate>        inline
        std::ostream& operator<<(std::ostream& out,
                                 const short_date_holder<ExtDate>& holder) {
            const DateLike<ExtDate>& d = holder.d;
            if (d == DateLike<ExtDate>()) {
                out << "null date";
            } else {
                FormatResetter resetter(out);
                Integer dd = d.dayOfMonth(), mm = Integer(d.month()),
                        yyyy = d.year();
                char filler = out.fill();
                out << std::setw(2) << std::setfill('0') << mm << "/";
                out << std::setw(2) << std::setfill('0') << dd << "/";
                out << yyyy;
                out.fill(filler);
            }
            return out;
        }
        template <class ExtDate>        inline
        std::ostream& operator<<(std::ostream& out,
                                 const long_date_holder<ExtDate>& holder) {
            const DateLike<ExtDate>& d = holder.d;
            if (d == DateLike<ExtDate>()) {
                out << "null date";
            } else {
                FormatResetter resetter(out);
                out << d.month() << " ";
                out << io::ordinal(d.dayOfMonth()) << ", ";
                out << d.year();
            }
            return out;
        }
        template <class ExtDate>        inline
        std::ostream& operator<<(std::ostream& out,
                                 const iso_date_holder<ExtDate>& holder) {
            const DateLike<ExtDate>& d = holder.d;
            if (d == DateLike<ExtDate>()) {
                out << "null date";
            } else {
                FormatResetter resetter(out);
                Integer dd = d.dayOfMonth(), mm = Integer(d.month()),
                        yyyy = d.year();
                out << yyyy << "-";
                out << std::setw(2) << std::setfill('0') << mm << "-";
                out << std::setw(2) << std::setfill('0') << dd;
            }
            return out;
        }
        template <class ExtDate>        inline
        std::ostream& operator<<(std::ostream& out,
                                 const formatted_date_holder<ExtDate>& holder) {
            QL_FAIL("don't want to use boost");
            //using namespace boost::gregorian;
            //const DateLike<ExtDate>& d = holder.d;
            //if (d == DateLike<ExtDate>()) {
            //    out << "null date";
            //} else {
            //    FormatResetter resetter(out);
            //    date boostDate(d.year(), d.month(), d.dayOfMonth());
            //    out.imbue(std::locale(std::locale(),
            //                          new date_facet(holder.f.c_str())));
            //    out << boostDate;
            //}
            return out;
        }

#ifdef QL_HIGH_RESOLUTION_DATE
        std::ostream& operator<<(std::ostream& out,
                                 const iso_datetime_holder& holder) {
            const DateLike<ExtDate>& d = holder.d;

            out << io::iso_date(d) << "T";
            FormatResetter resetter(out);
            const Hour hh= d.hours();
            const Minute mm = d.minutes();
            const Second s = d.seconds();
            const Millisecond millis = d.milliseconds();
            const Microsecond micros = d.microseconds();

            out << std::setw(2) << std::setfill('0') << hh << ":"
                << std::setw(2) << std::setfill('0') << mm << ":"
                << std::setw(2) << std::setfill('0') << s << ","
                << std::setw(3) << std::setfill('0') << millis
                << std::setw(3) << std::setfill('0') << micros;

            return out;
        }
#endif
    }

    namespace ext_io {
        template <class ExtDate>        inline 
        ext_detail::short_date_holder<ExtDate> short_date(const DateLike<ExtDate>& d) {
            return detail::short_date_holder(d);
        }
        template <class ExtDate>        inline 
        ext_detail::long_date_holder<ExtDate> long_date(const DateLike<ExtDate>& d) {
            return detail::long_date_holder(d);
        }
        template <class ExtDate>        inline 
        ext_detail::iso_date_holder<ExtDate> iso_date(const DateLike<ExtDate>& d) {
            return detail::iso_date_holder(d);
        }
        template <class ExtDate>        inline 
        ext_detail::formatted_date_holder<ExtDate> formatted_date(const DateLike<ExtDate>& d,
                                                     const std::string& f) {
            return detail::formatted_date_holder(d, f);
        }

#ifdef QL_HIGH_RESOLUTION_DATE
        detail::iso_datetime_holder iso_datetime(const DateLike<ExtDate>& d) {
            return detail::iso_datetime_holder(d);
        }
#endif
    }
}
