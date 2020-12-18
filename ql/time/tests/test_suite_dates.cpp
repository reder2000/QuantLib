/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2004, 2009 Ferdinando Ametrano
 Copyright (C) 2006 Katiuscia Manzoni
 Copyright (C) 2003 RiskMap srl
 Copyright (C) 2015 Maddalena Zanzi
 Copyright (c) 2015 Klaus Spanderen
 Copyright (C) 2020 Leonardo Arcari
 Copyright (C) 2020 Kline s.r.l.

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; IF not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <catch.hpp>
//#include "utilities.hpp"
#include <ql/time/date_like.hpp>
#include <ql/time/timeunit.hpp>
#include <ql/time/imm.hpp>
#include <ql/time/ecb.hpp>
#include <ql/time/asx.hpp>
#include <ql/time/ql_utilities_dataparsers.hpp>

#include <unordered_set>
//#include <boost/functional/hash.hpp>
#include <sstream>

using namespace QuantLib;
//using namespace boost::unit_test_framework;

#define BOOST_TEST_MESSAGE INFO
#define BOOST_FAIL(...) 
#define IF(a) CHECK_FALSE(a)

TEST_CASE("ecbDates", "[DateTest][hide]") {
//void DateTest::ecbDates() {
    BOOST_TEST_MESSAGE("Testing ECB dates...");

    std::set<Date> knownDates = ECB::knownDates();
    IF (knownDates.empty())
        BOOST_FAIL("\nempty EBC date vector");

    Size n = ECB::nextDates(Date::minDate()).size();
    IF (n != knownDates.size())
        BOOST_FAIL("\nnextDates(minDate) returns "  << n <<
                   " instead of " << knownDates.size() << " dates");

    std::set<Date>::const_iterator i;
    Date previousEcbDate = Date::minDate(),
         currentEcbDate, ecbDateMinusOne;
    for (i=knownDates.begin(); i!=knownDates.end(); ++i) {

        currentEcbDate = *i;
        IF (!ECB::isECBdate(currentEcbDate))
            BOOST_FAIL("\n" << currentEcbDate << " fails isECBdate check");

        ecbDateMinusOne = currentEcbDate-1;
        IF (ECB::isECBdate(ecbDateMinusOne))
            BOOST_FAIL("\n" << ecbDateMinusOne << " fails isECBdate check");

        IF (ECB::nextDate(ecbDateMinusOne)!=currentEcbDate)
            BOOST_FAIL("\n next EBC date following " << ecbDateMinusOne <<
                       " must be " << currentEcbDate);

        IF (ECB::nextDate(previousEcbDate)!=currentEcbDate)
            BOOST_FAIL("\n next EBC date following " << previousEcbDate <<
                       " must be " << currentEcbDate);

        previousEcbDate = currentEcbDate;
    }

    Date knownDate = *knownDates.begin();
    ECB::removeDate(knownDate);
    IF (ECB::isECBdate(knownDate))
        BOOST_FAIL("\neunable to remove an EBC date");
    ECB::addDate(knownDate);
    IF (!ECB::isECBdate(knownDate))
        BOOST_FAIL("\neunable to add an EBC date");

}
TEST_CASE("immDates", "[DateTest][hide]") {
//    void DateTest::immDates() {
    BOOST_TEST_MESSAGE("Testing IMM dates...");

    const std::string IMMcodes[] = {
        "F0", "G0", "H0", "J0", "K0", "M0", "N0", "Q0", "U0", "V0", "X0", "Z0",
        "F1", "G1", "H1", "J1", "K1", "M1", "N1", "Q1", "U1", "V1", "X1", "Z1",
        "F2", "G2", "H2", "J2", "K2", "M2", "N2", "Q2", "U2", "V2", "X2", "Z2",
        "F3", "G3", "H3", "J3", "K3", "M3", "N3", "Q3", "U3", "V3", "X3", "Z3",
        "F4", "G4", "H4", "J4", "K4", "M4", "N4", "Q4", "U4", "V4", "X4", "Z4",
        "F5", "G5", "H5", "J5", "K5", "M5", "N5", "Q5", "U5", "V5", "X5", "Z5",
        "F6", "G6", "H6", "J6", "K6", "M6", "N6", "Q6", "U6", "V6", "X6", "Z6",
        "F7", "G7", "H7", "J7", "K7", "M7", "N7", "Q7", "U7", "V7", "X7", "Z7",
        "F8", "G8", "H8", "J8", "K8", "M8", "N8", "Q8", "U8", "V8", "X8", "Z8",
        "F9", "G9", "H9", "J9", "K9", "M9", "N9", "Q9", "U9", "V9", "X9", "Z9"
    };

    Date counter = Date::minDate();
    // 10 years of futures must not exceed Date::maxDate
    Date last = Date::maxDate() - 121*Months;
    Date imm;

    while (counter<=last) {
        imm = IMM<Date>::nextDate(counter, false);

        // check that imm is greater than counter
        IF (imm<=counter)
            BOOST_FAIL("\n  "
                       << imm.weekday() << " " << imm
                       << " is not greater than "
                       << counter.weekday() << " " << counter);

        // check that imm is an IMM date
        IF (!IMM<Date>::isIMMdate(imm, false))
            BOOST_FAIL("\n  "
                       << imm.weekday() << " " << imm
                       << " is not an IMM date (calculated from "
                       << counter.weekday() << " " << counter << ")");

        // check that imm is <= to the next IMM date in the main cycle
        IF (imm>IMM<Date>::nextDate(counter, true))
            BOOST_FAIL("\n  "
                       << imm.weekday() << " " << imm
                       << " is not less than or equal to the next future in the main cycle "
                       << IMM<Date>::nextDate(counter, true));

        //// check that IF counter is an IMM date, then imm==counter
        //IF (IMM<Date>::isIMMdate(counter, false) && (imm!=counter))
        //    BOOST_FAIL("\n  "
        //               << counter.weekday() << " " << counter
        //               << " is already an IMM date, while nextIMM() returns "
        //               << imm.weekday() << " " << imm);

        // check that for every date IMMdate is the inverse of IMMcode
        IF (IMM<Date>::date(IMM<Date>::code(imm), counter) != imm)
            BOOST_FAIL("\n  "
                       << IMM<Date>::code(imm)
                       << " at calendar day " << counter
                       << " is not the IMM code matching " << imm);

        // check that for every date the 120 IMM codes refer to future dates
        for (int i=0; i<40; ++i) {
            IF (IMM<Date>::date(IMMcodes[i], counter)<counter)
                BOOST_FAIL("\n  "
                       << IMM<Date>::date(IMMcodes[i], counter)
                       << " is wrong for " << IMMcodes[i]
                       << " at reference date " << counter);
        }

        counter = counter + 1;
    }
}

TEST_CASE("asxDates", "[DateTest][hide]") {
//    void DateTest::asxDates() {
    BOOST_TEST_MESSAGE("Testing ASX dates...");

    const std::string ASXcodes[] = {
        "F0", "G0", "H0", "J0", "K0", "M0", "N0", "Q0", "U0", "V0", "X0", "Z0",
        "F1", "G1", "H1", "J1", "K1", "M1", "N1", "Q1", "U1", "V1", "X1", "Z1",
        "F2", "G2", "H2", "J2", "K2", "M2", "N2", "Q2", "U2", "V2", "X2", "Z2",
        "F3", "G3", "H3", "J3", "K3", "M3", "N3", "Q3", "U3", "V3", "X3", "Z3",
        "F4", "G4", "H4", "J4", "K4", "M4", "N4", "Q4", "U4", "V4", "X4", "Z4",
        "F5", "G5", "H5", "J5", "K5", "M5", "N5", "Q5", "U5", "V5", "X5", "Z5",
        "F6", "G6", "H6", "J6", "K6", "M6", "N6", "Q6", "U6", "V6", "X6", "Z6",
        "F7", "G7", "H7", "J7", "K7", "M7", "N7", "Q7", "U7", "V7", "X7", "Z7",
        "F8", "G8", "H8", "J8", "K8", "M8", "N8", "Q8", "U8", "V8", "X8", "Z8",
        "F9", "G9", "H9", "J9", "K9", "M9", "N9", "Q9", "U9", "V9", "X9", "Z9"
    };

    Date counter = Date::minDate();
    // 10 years of futures must not exceed Date::maxDate
    Date last = Date::maxDate() - 121 * Months;
    Date asx;

    while (counter <= last) {
        asx = ASX::nextDate(counter, false);

        // check that asx is greater than counter
        IF (asx <= counter)
            BOOST_FAIL("\n  "
            << asx.weekday() << " " << asx
            << " is not greater than "
            << counter.weekday() << " " << counter);

        // check that asx is an ASX date
        IF (!ASX::isASXdate(asx, false))
            BOOST_FAIL("\n  "
            << asx.weekday() << " " << asx
            << " is not an ASX date (calculated from "
            << counter.weekday() << " " << counter << ")");

        // check that asx is <= to the next ASX date in the main cycle
        IF (asx>ASX::nextDate(counter, true))
            BOOST_FAIL("\n  "
            << asx.weekday() << " " << asx
            << " is not less than or equal to the next future in the main cycle "
            << ASX::nextDate(counter, true));

        //// check that IF counter is an ASX date, then asx==counter
        //IF (ASX::isASXdate(counter, false) && (asx!=counter))
        //    BOOST_FAIL("\n  "
        //               << counter.weekday() << " " << counter
        //               << " is already an ASX date, while nextASX() returns "
        //               << asx.weekday() << " " << asx);

        // check that for every date ASXdate is the inverse of ASXcode
        IF (ASX::date(ASX::code(asx), counter) != asx)
            BOOST_FAIL("\n  "
            << ASX::code(asx)
            << " at calendar day " << counter
            << " is not the ASX code matching " << asx);

        // check that for every date the 120 ASX codes refer to future dates
        for (int i = 0; i<120; ++i) {
            IF (ASX::date(ASXcodes[i], counter)<counter)
                BOOST_FAIL("\n  "
                << ASX::date(ASXcodes[i], counter)
                << " is wrong for " << ASXcodes[i]
                << " at reference date " << counter);
        }

        counter = counter + 1;
    }
}

TEST_CASE("testConsistency", "[DateTest][hide]") {
//    void DateTest::testConsistency() {

    BOOST_TEST_MESSAGE("Testing dates...");

    serial_type minDate = Date::minDate().serialNumber()+1,
                      maxDate = Date::maxDate().serialNumber();

    serial_type dyold = Date(minDate-1).dayOfYear(),
                      dold  = Date(minDate-1).dayOfMonth(),
                      mold  = Date(minDate-1).month(),
                      yold  = Date(minDate-1).year(),
                      wdold = Date(minDate-1).weekday();

    for (serial_type i=minDate; i<=maxDate; i++) {
        Date t(i);
        serial_type serial = t.serialNumber();

        // check serial number consistency
        IF (serial != i)
            BOOST_FAIL("inconsistent serial number:\n"
                       << "    original:      " << i << "\n"
                       << "    date:          " << t << "\n"
                       << "    serial number: " << serial);

        Integer dy = t.dayOfYear(),
                d  = t.dayOfMonth(),
                m  = t.month(),
                y  = t.year(),
                wd = t.weekday();

        // check IF skipping any date
        IF (!((dy == dyold+1) ||
              (dy == 1 && dyold == 365 && !Date::isLeap(yold)) ||
              (dy == 1 && dyold == 366 && Date::isLeap(yold))))
            BOOST_FAIL("wrong day of year increment: \n"
                       << "    date: " << t << "\n"
                       << "    day of year: " << dy << "\n"
                       << "    previous:    " << dyold);
        dyold = dy;

        IF (!((d == dold+1 && m == mold   && y == yold) ||
              (d == 1      && m == mold+1 && y == yold) ||
              (d == 1      && m == 1      && y == yold+1)))
            BOOST_FAIL("wrong day,month,year increment: \n"
                       << "    date: " << t << "\n"
                       << "    day,month,year: "
                       << d << "," << Integer(m) << "," << y << "\n"
                       << "    previous:       "
                       << dold<< "," << Integer(mold) << "," << yold);
        dold = d; mold = m; yold = y;

        // check month definition
        IF ((m < 1 || m > 12))
            BOOST_FAIL("invalid month: \n"
                       << "    date:  " << t << "\n"
                       << "    month: " << Integer(m));

        // check day definition
        IF (d < 1)
            BOOST_FAIL("invalid day of month: \n"
                       << "    date:  " << t << "\n"
                       << "    day: " << d);
        IF (!((m == 1  && d <= 31) ||
              (m == 2  && d <= 28) ||
              (m == 2  && d == 29 && Date::isLeap(y)) ||
              (m == 3  && d <= 31) ||
              (m == 4  && d <= 30) ||
              (m == 5  && d <= 31) ||
              (m == 6  && d <= 30) ||
              (m == 7  && d <= 31) ||
              (m == 8  && d <= 31) ||
              (m == 9  && d <= 30) ||
              (m == 10 && d <= 31) ||
              (m == 11 && d <= 30) ||
              (m == 12 && d <= 31)))
            BOOST_FAIL("invalid day of month: \n"
                       << "    date:  " << t << "\n"
                       << "    day: " << d);

        // check weekday definition
        IF (!((Integer(wd) == Integer(wdold+1)) ||
              (Integer(wd) == 1 && Integer(wdold) == 7)))
            BOOST_FAIL("invalid weekday: \n"
                       << "    date:  " << t << "\n"
                       << "    weekday:  " << Integer(wd) << "\n"
                       << "    previous: " << Integer(wdold));
        wdold = wd;

        // create the same date with a different constructor
        Date s(d,Month(m),y);
        // check serial number consistency
        serial = s.serialNumber();
        IF (serial != i)
            BOOST_FAIL("inconsistent serial number:\n"
                       << "    date:          " << t << "\n"
                       << "    serial number: " << i << "\n"
                       << "    cloned date:   " <<  s << "\n"
                       << "    serial number: " << serial);
    }

}
TEST_CASE("isoDates", "[DateTest][hide]") {
    //void DateTest::isoDates() {
    BOOST_TEST_MESSAGE("Testing ISO dates...");
    std::string input_date("2006-01-15");
    Date d = DateParser<Date>::parseISO(input_date);
    IF ((d.dayOfMonth() != 15 ||
        d.month() != January ||
        d.year() != 2006))
        BOOST_FAIL("Iso date failed\n"
                   << " input date:    " << input_date << "\n"
                   << " day of month:  " << d.dayOfMonth() << "\n"
                   << " month:         " << d.month() << "\n"
                   << " year:          " << d.year()) ; 
    
}
TEST_CASE("parseDates", "[DateTest][hide]") {
//    void DateTest::parseDates() {
    BOOST_TEST_MESSAGE("Testing parsing of dates...");

    std::string input_date("2006-01-15");
    Date d = DateParser<Date>::parseFormatted(input_date, "%Y-%m-%d");
    IF (d != Date(15, January, 2006)) 
        BOOST_FAIL("date parsing failed\n"
                   << " input date:  " << input_date << "\n"
                   << " parsed date: " << d);
    

    input_date = "12/02/2012";
    d = DateParser<Date>::parseFormatted(input_date, "%m/%d/%Y");
    IF (d != Date(2, December, 2012)) 
        BOOST_FAIL("date parsing failed\n"
                   << " input date:  " << input_date << "\n"
                   << " parsed date: " << d);
    
    d = DateParser<Date>::parseFormatted(input_date, "%d/%m/%Y");
    IF (d != Date(12, February, 2012)) 
        BOOST_FAIL("date parsing failed\n"
                   << " input date:  " << input_date << "\n"
                   << " parsed date: " << d);
    

    input_date = "20011002";
    d = DateParser<Date>::parseFormatted(input_date, "%Y%m%d");
    IF (d != Date(2, October, 2001)) 
        BOOST_FAIL("date parsing failed\n"
                   << " input date:  " << input_date << "\n"
                   << " parsed date: " << d);
    
}
TEST_CASE("intraday", "[DateTest][hide]") {
//    void DateTest::intraday() {
#ifdef QL_HIGH_RESOLUTION_DATE

    BOOST_TEST_MESSAGE("Testing intraday information of dates...");

    const Date d1 = Date(12, February, 2015, 10, 45, 12, 1234, 76253);

    BOOST_CHECK_MESSAGE(d1.year() == 2015, "failed to reproduce year");
    BOOST_CHECK_MESSAGE(d1.month() == February, "failed to reproduce month");
    BOOST_CHECK_MESSAGE(d1.dayOfMonth() == 12, "failed to reproduce day");
    BOOST_CHECK_MESSAGE(d1.hours() == 10, "failed to reproduce hour of day");
    BOOST_CHECK_MESSAGE(d1.minutes() == 45,
        "failed to reproduce minute of hour");
    BOOST_CHECK_MESSAGE(d1.seconds() == 13,
        "failed to reproduce second of minute");

    IF (Date::ticksPerSecond() == 1000)
        BOOST_CHECK_MESSAGE(d1.fractionOfSecond() == 0.234,
            "failed to reproduce fraction of second");
    else IF (Date::ticksPerSecond() >= 1000000)
        BOOST_CHECK_MESSAGE(d1.fractionOfSecond() == (234000 + 76253)/1000000.0,
        "failed to reproduce fraction of second");

    IF (Date::ticksPerSecond() >= 1000)
        BOOST_CHECK_MESSAGE(d1.milliseconds() == 234 + 76,
            "failed to reproduce number of milliseconds");

    IF (Date::ticksPerSecond() >= 1000000)
        BOOST_CHECK_MESSAGE(d1.microseconds() == 253,
            "failed to reproduce number of microseconds");

    const Date d2 = Date(28, February, 2015, 50, 165, 476, 1234, 253);
    BOOST_CHECK_MESSAGE(d2.year() == 2015, "failed to reproduce year");
    BOOST_CHECK_MESSAGE(d2.month() == March, "failed to reproduce month");
    BOOST_CHECK_MESSAGE(d2.dayOfMonth() == 2, "failed to reproduce day");
    BOOST_CHECK_MESSAGE(d2.hours() == 4, "failed to reproduce hour of day");
    BOOST_CHECK_MESSAGE(d2.minutes() == 52,
        "failed to reproduce minute of hour");
    BOOST_CHECK_MESSAGE(d2.seconds() == 57,
        "failed to reproduce second of minute");

    IF (Date::ticksPerSecond() >= 1000)
        BOOST_CHECK_MESSAGE(d2.milliseconds() == 234,
            "failed to reproduce number of milliseconds");
    IF (Date::ticksPerSecond() >= 1000000)
        BOOST_CHECK_MESSAGE(d2.microseconds() == 253,
            "failed to reproduce number of microseconds");

    std::ostringstream s;
    s << io::iso_datetime(Date(7, February, 2015, 1, 4, 2, 3, 4));

    BOOST_CHECK_MESSAGE(s.str() == std::string("2015-02-07T01:04:02,003004"),
        "datetime to string failed to reproduce expected result");

#endif
}
/* FIXME 
TEST_CASE("canHash", "[DateTest][hide]") {
//    void DateTest::canHash() {
    BOOST_TEST_MESSAGE("Testing hashing of dates...");

    Date start_date = Date(1, Jan, 2020);
    int nb_tests = 500;

        hash<Date> hasher;

    // Check hash values
    for (int i = 0; i < nb_tests; ++i) {
        for (int j = 0; j < nb_tests; ++j) {
            Date lhs = start_date + i;
            Date rhs = start_date + j;

            IF (lhs == rhs && hasher(lhs) != hasher(rhs)) {
                BOOST_FAIL("Equal dates are expected to have same hash value\n"
                           << "rhs = " << lhs << '\n'
                           << "lhs = " << rhs << '\n'
                           << "hash(lhs) = " << hasher(lhs) << '\n'
                           << "hash(rhs) = " << hasher(rhs) << '\n');
            }

            IF (lhs != rhs && hasher(lhs) == hasher(rhs)) {
                BOOST_FAIL("Different dates are expected to have different hash value\n"
                           << "rhs = " << lhs << '\n'
                           << "lhs = " << rhs << '\n'
                           << "hash(lhs) = " << hasher(lhs) << '\n'
                           << "hash(rhs) = " << hasher(rhs) << '\n');
            }
        }
    }

    // Check IF Date can be used as unordered_set key
    boost::unordered_set<Date> set;
    set.insert(start_date);

    IF (set.count(start_date) == 0) {
        BOOST_FAIL("Expected to find date " << start_date << " in unordered_set\n");
    }
}
*/
//test_suite* DateTest::suite(SpeedLevel speed) {
//    test_suite* suite = BOOST_TEST_SUITE("Date tests");
//
//    suite->add(QUANTLIB_TEST_CASE(&DateTest::testConsistency));
//    suite->add(QUANTLIB_TEST_CASE(&DateTest::ecbDates));
//    suite->add(QUANTLIB_TEST_CASE(&DateTest::immDates));
//    suite->add(QUANTLIB_TEST_CASE(&DateTest::isoDates));
//    #ifndef QL_PATCH_SOLARIS
//    suite->add(QUANTLIB_TEST_CASE(&DateTest::parseDates));
//    #endif
//    suite->add(QUANTLIB_TEST_CASE(&DateTest::intraday));
//    suite->add(QUANTLIB_TEST_CASE(&DateTest::canHash));
//
//    IF (speed <= Fast) {
//        suite->add(QUANTLIB_TEST_CASE(&DateTest::asxDates));
//    }
//
//    return suite;
//}
