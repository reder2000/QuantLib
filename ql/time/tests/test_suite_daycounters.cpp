/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003 RiskMap srl
 Copyright (C) 2006 Piter Dias
 Copyright (C) 2012 Simon Shakeshaft
 Copyright (c) 2015 Klaus Spanderen

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


//#include "daycounters.hpp"
//#include "utilities.hpp"
#include <ql/time/daycounters/actual360.hpp>
#include <ql/time/daycounters/actualactual.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include <ql/time/daycounters/one.hpp>
#include <ql/time/daycounters/simpledaycounter.hpp>
#include <ql/time/daycounters/business252.hpp>
#include <ql/time/daycounters/thirty360.hpp>
#include <ql/time/daycounters/thirty365.hpp>
#include <ql/time/calendars/brazil.hpp>
#include <ql/time/calendars/canada.hpp>
#include <ql/time/calendars/unitedstates.hpp>
#include <ql/time/schedule.hpp>
#include "boost_to_catch.h"
#include "pseudo_dates.h"
#include <iomanip>

using namespace QuantLib;
//using namespace boost::unit_test_framework;
using eDate = QuantLibDate;
using DLe = DateLike<eDate>;
using DAe = DateAdaptor<eDate>;

namespace day_counters_test {

    struct SingleCase {
        SingleCase(ActualActual<eDate>::Convention convention,
            const eDate& start,
            const eDate& end,
            const eDate& refStart,
            const eDate& refEnd,
            Time result)
            : convention(convention), start(start), end(end),
            refStart(refStart), refEnd(refEnd), result(result) {}
        SingleCase(ActualActual<eDate>::Convention convention,
            const eDate& start,
            const eDate& end,
            Time result)
            : convention(convention), start(start), end(end),
            refStart(eDate()), refEnd(eDate()), result(result) {}
        ActualActual<eDate>::Convention convention;
        eDate start;
        eDate end;
        eDate refStart;
        eDate refEnd;
        Time result;
    };

    Time ISMAYearFractionWithReferenceDates(
        const DayCounter<eDate>& dayCounter, eDate start, eDate end, eDate refStart, eDate refEnd) {
        Real referenceDayCount = Real(dayCounter.dayCount(refStart, refEnd));
        // guess how many coupon periods per year:
        Integer couponsPerYear = (Integer)(0.5 + 365.0 / referenceDayCount);
        // the above is good enough for annual or semi annual payments.
        return Real(dayCounter.dayCount(start, end))
            / (referenceDayCount*couponsPerYear);
    }

    Time actualActualDaycountComputation(const Schedule<eDate>& schedule, eDate start, eDate end) {

        DayCounter daycounter = ActualActual(ActualActual<eDate>::ISMA, schedule);
        Time yearFraction = 0.0;

        for (Size i = 1; i < schedule.size() - 1; i++) {
            eDate referenceStart = schedule.date(i);
            eDate referenceEnd = schedule.date(i+1);
            if (to_DateLike(start) < referenceEnd && to_DateLike(end) > referenceStart) {
                yearFraction += ISMAYearFractionWithReferenceDates(
                    daycounter,
                    (to_DateLike(start) > referenceStart) ? start : referenceStart,
                    (to_DateLike(end) < referenceEnd) ? end : referenceEnd,
                    referenceStart,
                    referenceEnd
                );
            };
        }
        return yearFraction;
    }

}


TEST_CASE("testActualActual", "[DayCounterTest][hide]") {

    BOOST_TEST_MESSAGE("Testing actual/actual day counters...");

    using namespace day_counters_test;

    SingleCase testCases[] = {
        // first example
        SingleCase(ActualActual<eDate>::ISDA,
                   DAe::Date(1,November,2003), DAe::Date(1,May,2004),
                   0.497724380567),
        SingleCase(ActualActual<eDate>::ISMA,
                   DAe::Date(1,November,2003), DAe::Date(1,May,2004),
                   DAe::Date(1,November,2003), DAe::Date(1,May,2004),
                   0.500000000000),
        SingleCase(ActualActual<eDate>::AFB,
                   DAe::Date(1,November,2003), DAe::Date(1,May,2004),
                   0.497267759563),
        // short first calculation period (first period)
        SingleCase(ActualActual<eDate>::ISDA,
                   DAe::Date(1,February,1999), DAe::Date(1,July,1999),
                   0.410958904110),
        SingleCase(ActualActual<eDate>::ISMA,
                   DAe::Date(1,February,1999), DAe::Date(1,July,1999),
                   DAe::Date(1,July,1998), DAe::Date(1,July,1999),
                   0.410958904110),
        SingleCase(ActualActual<eDate>::AFB,
                   DAe::Date(1,February,1999), DAe::Date(1,July,1999),
                   0.410958904110),
        // short first calculation period (second period)
        SingleCase(ActualActual<eDate>::ISDA,
                   DAe::Date(1,July,1999), DAe::Date(1,July,2000),
                   1.001377348600),
        SingleCase(ActualActual<eDate>::ISMA,
                   DAe::Date(1,July,1999), DAe::Date(1,July,2000),
                   DAe::Date(1,July,1999), DAe::Date(1,July,2000),
                   1.000000000000),
        SingleCase(ActualActual<eDate>::AFB,
                   DAe::Date(1,July,1999), DAe::Date(1,July,2000),
                   1.000000000000),
        // long first calculation period (first period)
        SingleCase(ActualActual<eDate>::ISDA,
                   DAe::Date(15,August,2002), DAe::Date(15,July,2003),
                   0.915068493151),
        SingleCase(ActualActual<eDate>::ISMA,
                   DAe::Date(15,August,2002), DAe::Date(15,July,2003),
                   DAe::Date(15,January,2003), DAe::Date(15,July,2003),
                   0.915760869565),
        SingleCase(ActualActual<eDate>::AFB,
                   DAe::Date(15,August,2002), DAe::Date(15,July,2003),
                   0.915068493151),
        // long first calculation period (second period)
        /* Warning: the ISDA case is in disagreement with mktc1198.pdf */
        SingleCase(ActualActual<eDate>::ISDA,
                   DAe::Date(15,July,2003), DAe::Date(15,January,2004),
                   0.504004790778),
        SingleCase(ActualActual<eDate>::ISMA,
                   DAe::Date(15,July,2003), DAe::Date(15,January,2004),
                   DAe::Date(15,July,2003), DAe::Date(15,January,2004),
                   0.500000000000),
        SingleCase(ActualActual<eDate>::AFB,
                   DAe::Date(15,July,2003), DAe::Date(15,January,2004),
                   0.504109589041),
        // short final calculation period (penultimate period)
        SingleCase(ActualActual<eDate>::ISDA,
                   DAe::Date(30,July,1999), DAe::Date(30,January,2000),
                   0.503892506924),
        SingleCase(ActualActual<eDate>::ISMA,
                   DAe::Date(30,July,1999), DAe::Date(30,January,2000),
                   DAe::Date(30,July,1999), DAe::Date(30,January,2000),
                   0.500000000000),
        SingleCase(ActualActual<eDate>::AFB,
                   DAe::Date(30,July,1999), DAe::Date(30,January,2000),
                   0.504109589041),
        // short final calculation period (final period)
        SingleCase(ActualActual<eDate>::ISDA,
                   DAe::Date(30,January,2000), DAe::Date(30,June,2000),
                   0.415300546448),
        SingleCase(ActualActual<eDate>::ISMA,
                   DAe::Date(30,January,2000), DAe::Date(30,June,2000),
                   DAe::Date(30,January,2000), DAe::Date(30,July,2000),
                   0.417582417582),
        SingleCase(ActualActual<eDate>::AFB,
                   DAe::Date(30,January,2000), DAe::Date(30,June,2000),
                   0.41530054644)
    };

    Size n = sizeof(testCases)/sizeof(SingleCase);
    for (Size i=0; i<n; i++) {
        ActualActual<eDate> dayCounter(testCases[i].convention);
        eDate d1 = testCases[i].start,
            d2 = testCases[i].end,
            rd1 = testCases[i].refStart,
            rd2 = testCases[i].refEnd;
        Time calculated = dayCounter.yearFraction(d1,d2,rd1,rd2);

        IF(std::fabs(calculated - testCases[i].result) > 1.0e-10)
        BOOST_FAIL({
            std::ostringstream period, refPeriod;
            period << "period: " << d1 << " to " << d2;
            if (testCases[i].convention == ActualActual<eDate>::ISMA)
                refPeriod << "referencePeriod: " << rd1 << " to " << rd2;
            BOOST_ERROR(dayCounter.name()
                        << ":\n"
                        << period.str() << "\n"
                        << refPeriod.str() << "\n"
                        << std::setprecision(10) << "    calculated: " << calculated << "\n"
                        << "    expected:   " << testCases[i].result);
        });
    }
}

TEST_CASE("testActualActualWithSemiannualSchedule", "[DayCounterTest][hide]") {

    BOOST_TEST_MESSAGE("Testing actual/actual with schedule "
                       "for undefined semiannual reference periods...");

    using namespace day_counters_test;

    Calendar<eDate> calendar = UnitedStates<eDate>();
    eDate fromDate = DateAdaptor<eDate>::Date(10, January, 2017);
    eDate firstCoupon = DateAdaptor<eDate>::Date(31, August, 2017);
    eDate quasiCoupon = DateAdaptor<eDate>::Date(28, February, 2017);
    eDate quasiCoupon2 = DateAdaptor<eDate>::Date(31, August, 2016);

    Schedule<eDate> schedule = MakeSchedule<eDate>()
        .from(fromDate)
        .withFirstDate(firstCoupon)
        .to(DAe::Date(28, February, 2026))
        .withFrequency(Semiannual)
        .withCalendar(calendar)
        .withConvention(Unadjusted)
        .backwards().endOfMonth(true);

    eDate testDate = schedule.date(1);
    DayCounter<eDate> dayCounter = ActualActual<eDate>(ActualActual<eDate>::ISMA, schedule);
    DayCounter<eDate> dayCounterNoSchedule = ActualActual<eDate>(ActualActual<eDate>::ISMA);

    eDate referencePeriodStart = schedule.date(1);
    eDate referencePeriodEnd = schedule.date(2);

    //Test
    QL_ASSERT(dayCounter.yearFraction(referencePeriodStart,
                                      referencePeriodStart) == 0.0,
              "This should be zero."
    );
    QL_ASSERT(dayCounterNoSchedule.yearFraction(referencePeriodStart,
                                                referencePeriodStart) == 0.0,
              "This should be zero"
    );
    QL_ASSERT(dayCounterNoSchedule.yearFraction(referencePeriodStart,
                                                referencePeriodStart,
                                                referencePeriodStart,
                                                referencePeriodStart) == 0.0,
              "This should be zero"
    );
    QL_ASSERT(dayCounter.yearFraction(referencePeriodStart,
                                      referencePeriodEnd) == 0.5,
              "This should be exact using schedule; {} to {} Should be 0.5", referencePeriodStart,
              referencePeriodEnd
    );
    QL_ASSERT(dayCounterNoSchedule.yearFraction(referencePeriodStart,
                                                referencePeriodEnd,
                                                referencePeriodStart,
                                                referencePeriodEnd) == 0.5,
              "This should be exact for explicit reference "
              "periods with no schedule");


    while (to_DateLike(testDate) < referencePeriodEnd) {
        Time difference =
            dayCounter.yearFraction(testDate, referencePeriodEnd,
                                    referencePeriodStart, referencePeriodEnd) -
            dayCounter.yearFraction(testDate, referencePeriodEnd);
        IFe (std::fabs(difference) > 1.0e-10) {
            BOOST_ERROR("Failed to correctly use the schedule "
                        "to find the reference period for Act/Act");
        };
        testDate = calendar.advance(testDate, 1, Days);
    }

    //Test long first coupon
    Real calculatedYearFraction =
        dayCounter.yearFraction(fromDate, firstCoupon);
    Real expectedYearFraction =
        0.5 + ((Real) dayCounter.dayCount(fromDate, quasiCoupon))
          /(2*dayCounter.dayCount(quasiCoupon2, quasiCoupon));

    QL_ASSERT(std::fabs(calculatedYearFraction-expectedYearFraction) < 1.0e-10,
              "Failed to compute the expected year fraction "
              "\n expected: {}\n calculated: {}",
              expectedYearFraction ,  calculatedYearFraction);

    // test multiple periods

    schedule = MakeSchedule<eDate>()
        .from(DAe::Date(10, January, 2017))
        .withFirstDate(DAe::Date(31, August, 2017))
        .to(DAe::Date(28, February, 2026))
        .withFrequency(Semiannual)
        .withCalendar(calendar)
        .withConvention(Unadjusted)
        .backwards().endOfMonth(false);

    eDate periodStartDate = schedule.date(1);
    eDate periodEndDate = schedule.date(2);

    dayCounter = ActualActual<eDate>(ActualActual<eDate>::ISMA, schedule);

    while (to_DateLike(periodEndDate) < schedule.date(schedule.size()-2)) {
        Time expected =
            actualActualDaycountComputation(schedule,
                                            periodStartDate,
                                            periodEndDate);
        Time calculated = dayCounter.yearFraction(periodStartDate,
                                                  periodEndDate);

        IFe (std::fabs(expected - calculated) > 1e-8) {
            BOOST_ERROR("Failed to compute the correct year fraction "
                        "given a schedule: " << periodStartDate <<
                        " to " << periodEndDate <<
                        "\n expected: " << expected <<
                        " calculated: " << calculated);
        }
        periodEndDate = calendar.advance(periodEndDate, 1, Days);
    }
}

TEST_CASE("testActualActualWithAnnualSchedule", "[DayCounterTest][hide]") {

    BOOST_TEST_MESSAGE("Testing actual/actual with schedule "
                       "for undefined annual reference periods...");

    using namespace day_counters_test;

    // Now do an annual schedule
    Calendar<eDate> calendar = UnitedStates<eDate>();
    Schedule<eDate> schedule = MakeSchedule<eDate>()
        .from(DAe::Date(10, January, 2017))
        .withFirstDate(DAe::Date(31, August, 2017))
        .to(DAe::Date(28, February, 2026))
        .withFrequency(Annual)
        .withCalendar(calendar)
        .withConvention(Unadjusted)
        .backwards().endOfMonth(false);

    eDate referencePeriodStart = schedule.date(1);
    eDate referencePeriodEnd = schedule.date(2);

    eDate testDate = schedule.date(1);
    DayCounter dayCounter = ActualActual(ActualActual<eDate>::ISMA, schedule);

    while (to_DateLike(testDate) < referencePeriodEnd) {
        Time difference =
            ISMAYearFractionWithReferenceDates(dayCounter,
                                               testDate, referencePeriodEnd,
                                               referencePeriodStart, referencePeriodEnd) -
            dayCounter.yearFraction(testDate, referencePeriodEnd);
        IFe (std::fabs(difference) > 1.0e-10) {
            BOOST_ERROR("Failed to correctly use the schedule "
                        "to find the reference period for Act/Act:\n"
                        << testDate << " to " << referencePeriodEnd
                        << "\n Ref: " << referencePeriodStart
                        << " to " << referencePeriodEnd);
        }

        testDate = calendar.advance(testDate, 1, Days);
    }
}

TEST_CASE("testActualActualWithSchedule", "[DayCounterTest][hide]") {

    BOOST_TEST_MESSAGE("Testing actual/actual day counter with schedule...");

    using namespace day_counters_test;

    // long first coupon
    eDate issueDateExpected = DAe::Date(17, January, 2017);
    eDate firstCouponDateExpected = DAe::Date(31, August, 2017);

    Schedule<eDate> schedule = MakeSchedule<eDate>()
        .from(issueDateExpected)
        .withFirstDate(firstCouponDateExpected)
        .to(DAe::Date(28, February, 2026))
        .withFrequency(Semiannual)
        .withCalendar(Canada<eDate>())
        .withConvention(Unadjusted)
        .backwards()
        .endOfMonth();

    eDate issueDate = schedule.date(0);
    QL_REQUIRE(to_DateLike(issueDate) == issueDateExpected, "This is not the expected issue date {} expected {}",
               issueDate, issueDateExpected);
    eDate firstCouponDate = schedule.date(1);
    QL_REQUIRE(to_DateLike(firstCouponDate) == firstCouponDateExpected,
               "This is not the expected first coupon date {} expected: {}",
               firstCouponDate, firstCouponDateExpected);

    //Make thw quasi coupon dates:
    eDate quasiCouponDate2 = schedule.calendar().advance(firstCouponDate,
        -schedule.tenor(),
        schedule.businessDayConvention(),
        schedule.endOfMonth());
    eDate quasiCouponDate1 = schedule.calendar().advance(quasiCouponDate2,
        -schedule.tenor(),
        schedule.businessDayConvention(),
        schedule.endOfMonth());

    eDate quasiCouponDate1Expected = DAe::Date(31, August, 2016);
    eDate quasiCouponDate2Expected = DAe::Date(28, February, 2017);

    QL_REQUIRE(to_DateLike(quasiCouponDate2) == quasiCouponDate2Expected,
               "Expected {} as the later quasi coupon date but received {}",
               quasiCouponDate2Expected  , quasiCouponDate2);
    QL_REQUIRE(to_DateLike(quasiCouponDate1) == quasiCouponDate1Expected,
               "Expected {} as the earlier quasi coupon date but received {}",
               quasiCouponDate1Expected, quasiCouponDate1);

    DayCounter dayCounter = ActualActual<eDate>(ActualActual<eDate>::ISMA, schedule);

    // full coupon
    Time t_with_reference = dayCounter.yearFraction(
        issueDate, firstCouponDate,
        quasiCouponDate2, firstCouponDate
    );
    Time t_no_reference = dayCounter.yearFraction(
        issueDate,
        firstCouponDate
    );
    Time t_total =
        ISMAYearFractionWithReferenceDates(dayCounter,
                                           issueDate, quasiCouponDate2,
                                           quasiCouponDate1, quasiCouponDate2)
        + 0.5;
    Time expected = 0.6160220994;


    IFe (std::fabs(t_total - expected) > 1.0e-10) {
        BOOST_ERROR("Failed to reproduce expected time:\n"
            << std::setprecision(10)
            << "    calculated: " << t_total << "\n"
            << "    expected:   " << expected);
    }
    IFe (std::fabs(t_with_reference -expected) > 1.0e-10) {
        BOOST_ERROR("Failed to reproduce expected time:\n"
                   << std::setprecision(10)
                   << "    calculated: " << t_with_reference << "\n"
                   << "    expected:   " << expected);
    }
    IFe (std::fabs(t_no_reference - t_with_reference) > 1.0e-10) {
        BOOST_ERROR("Should produce the same time "
                    "whether or not references are present");
    }

    // settlement date in the first quasi-period
    eDate settlementDate = DAe::Date(29, January, 2017);

    t_with_reference = ISMAYearFractionWithReferenceDates(
        dayCounter,
        issueDate, settlementDate,
        quasiCouponDate1, quasiCouponDate2
    );
    t_no_reference = dayCounter.yearFraction(issueDate, settlementDate);
    Time t_expected_first_qp = 0.03314917127071823; //12.0/362
    IFe (std::fabs(t_with_reference - t_expected_first_qp) > 1.0e-10) {
        BOOST_ERROR("Failed to reproduce expected time:\n"
                   << std::setprecision(10)
                   << "    calculated: " << t_no_reference << "\n"
                   << "    expected:   " << t_expected_first_qp);
    }
    IFe (std::fabs(t_no_reference - t_with_reference) > 1.0e-10) {
        BOOST_ERROR("Should produce the same time "
                    "whether or not references are present");
    }
    Time t2 = dayCounter.yearFraction(settlementDate, firstCouponDate);
    IFe (std::fabs(t_expected_first_qp + t2 - expected) > 1.0e-10) {
        BOOST_ERROR("Sum of quasiperiod2 split is not consistent");
    }

    // settlement date in the second quasi-period
    settlementDate = DAe::Date(29, July, 2017);

    /*T = dayCounter.yearFraction(issueDate,
                                settlementDate,
                                quasiCouponDate2,
                                firstCouponDate);
    t1 = dayCounter.yearFraction(issueDate,
                                 quasiCouponDate2,
                                 quasiCouponDate1,
                                 quasiCouponDate2);
    Time t2 = dayCounter.yearFraction(quasiCouponDate2,
                                      settlementDate,
                                      quasiCouponDate2,
                                      firstCouponDate);*/
    t_no_reference = dayCounter.yearFraction(issueDate, settlementDate);
    t_with_reference = ISMAYearFractionWithReferenceDates(
        dayCounter,
        issueDate, quasiCouponDate2,
        quasiCouponDate1, quasiCouponDate2
    ) + ISMAYearFractionWithReferenceDates(
        dayCounter,
        quasiCouponDate2, settlementDate,
        quasiCouponDate2, firstCouponDate
    );
    IFe (std::fabs(t_no_reference - t_with_reference) > 1.0e-10) {
        BOOST_ERROR("These two cases should be identical");
    };
    t2 = dayCounter.yearFraction(settlementDate, firstCouponDate);


    IFe (std::fabs(t_total-(t_no_reference+t2)) > 1.0e-10) {
        BOOST_ERROR("Failed to reproduce expected time:\n"
                   << std::setprecision(10)
                   << "    calculated: " << t_total << "\n"
                   << "    expected:   " << t_no_reference+t2);
    }
}

TEST_CASE("testSimple", "[DayCounterTest][hide]") {

    BOOST_TEST_MESSAGE("Testing simple day counter...");

    Period p[] = { Period(3,Months), Period(6,Months), Period(1,Years) };
    Time expected[] = { 0.25, 0.5, 1.0 };
    Size n = sizeof(p)/sizeof(Period);

    // 4 years should be enough
    eDate first = DAe::Date(1, January, 2002), last = DAe::Date(31, December, 2005);
    DayCounter<eDate> dayCounter = SimpleDayCounter<eDate>();

    for (auto start = to_DateLike(first); start <= last; start++) {
        for (Size i=0; i<n; i++) {
            eDate end = start + p[i];
            Time calculated = dayCounter.yearFraction(start.asExtDate(),end);
            IFe (std::fabs(calculated-expected[i]) > 1.0e-12)  {
                BOOST_ERROR("from " << start << " to " << end << ":\n"
                           << std::setprecision(12)
                           << "    calculated: " << calculated << "\n"
                           << "    expected:   " << expected[i]);
            }
        }
    }
}

TEST_CASE("testOne", "[DayCounterTest][hide]") {

    BOOST_TEST_MESSAGE("Testing 1/1 day counter...");

    Period p[] = { Period(3,Months), Period(6,Months), Period(1,Years) };
    Time expected[] = { 1.0, 1.0, 1.0 };
    Size n = sizeof(p)/sizeof(Period);

    // 1 years should be enough
    eDate first = DAe::Date(1, January, 2004), last = DAe::Date(31, December, 2004);
    DayCounter<eDate> dayCounter = OneDayCounter<eDate>();

    for (auto start = to_DateLike(first); start <= last; start++) {
        for (Size i=0; i<n; i++) {
            eDate end = start + p[i];
            Time calculated = dayCounter.yearFraction(start,end);
            IFe(std::fabs(calculated - expected[i]) > 1.0e-12)
            {
                BOOST_ERROR("from " << start << " to " << end << ":\n"
                           << std::setprecision(12)
                           << "    calculated: " << calculated << "\n"
                           << "    expected:   " << expected[i]);
            }
        }
    }
}

TEST_CASE("testBusiness252", "[DayCounterTest][hide]") {

    BOOST_TEST_MESSAGE("Testing business/252 day counter...");

    std::vector<eDate> testDates;
    testDates.push_back(DAe::Date(1,February,2002));
    testDates.push_back(DAe::Date(4,February,2002));
    testDates.push_back(DAe::Date(16,May,2003));
    testDates.push_back(DAe::Date(17,December,2003));
    testDates.push_back(DAe::Date(17,December,2004));
    testDates.push_back(DAe::Date(19,December,2005));
    testDates.push_back(DAe::Date(2,January,2006));
    testDates.push_back(DAe::Date(13,March,2006));
    testDates.push_back(DAe::Date(15,May,2006));
    testDates.push_back(DAe::Date(17,March,2006));
    testDates.push_back(DAe::Date(15,May,2006));
    testDates.push_back(DAe::Date(26,July,2006));
    testDates.push_back(DAe::Date(28,June,2007));
    testDates.push_back(DAe::Date(16,September,2009));
    testDates.push_back(DAe::Date(26,July,2016));

    Time expected[] = {
        0.0039682539683,
        1.2738095238095,
        0.6031746031746,
        0.9960317460317,
        1.0000000000000,
        0.0396825396825,
        0.1904761904762,
        0.1666666666667,
        -0.1507936507937,
        0.1507936507937,
        0.2023809523810,
        0.912698412698,
        2.214285714286,
        6.84126984127
        };

    DayCounter<eDate> dayCounter1 = Business252<eDate>(Brazil<eDate>());

    Time calculated;

    for (Size i=1; i<testDates.size(); i++) {
        calculated = dayCounter1.yearFraction(testDates[i-1],testDates[i]);
        IFe (std::fabs(calculated-expected[i-1]) > 1.0e-12) {
                BOOST_ERROR("from " << testDates[i-1]
                            << " to " << testDates[i] << ":\n"
                            << std::setprecision(14)
                            << "    calculated: " << calculated << "\n"
                            << "    expected:   " << expected[i-1]);
        }
    }

    DayCounter<eDate> dayCounter2 = Business252<eDate>();

    for (Size i=1; i<testDates.size(); i++) {
        calculated = dayCounter2.yearFraction(testDates[i-1],testDates[i]);
        IFe (std::fabs(calculated-expected[i-1]) > 1.0e-12) {
                BOOST_ERROR("from " << testDates[i-1]
                            << " to " << testDates[i] << ":\n"
                            << std::setprecision(14)
                            << "    calculated: " << calculated << "\n"
                            << "    expected:   " << expected[i-1]);
        }
    }
}

TEST_CASE("testThirty365", "[DayCounterTest][hide]") {

    BOOST_TEST_MESSAGE("Testing 30/365 day counter...");

    eDate d1 = DAe::Date(17, June, 2011), d2 = DAe::Date(30, December, 2012);
    DayCounter<eDate> dayCounter = Thirty365<eDate>();

    BigInteger days = dayCounter.dayCount(d1,d2);
    IFe (days != 553) {
        BOOST_FAIL("from " << d1 << " to " << d2 << ":\n"
                   << "    calculated: " << days << "\n"
                   << "    expected:   " << 553);
    }

    Time t = dayCounter.yearFraction(d1,d2);
    Time expected = 553/365.0;
    IFe (std::fabs(t-expected) > 1.0e-12) {
        BOOST_FAIL("from " << d1 << " to " << d2 << ":\n"
                   << std::setprecision(12)
                   << "    calculated: " << t << "\n"
                   << "    expected:   " << expected);
    }
}

TEST_CASE("testThirty360_BondBasis", "[DayCounterTest][hide]") {

    BOOST_TEST_MESSAGE("Testing thirty/360 day counter (Bond Basis)...");

    // http://www.isda.org/c_and_a/docs/30-360-2006ISDADefs.xls
    // Source: 2006 ISDA Definitions, Sec. 4.16 (f)
    // 30/360 (or Bond Basis)

    DayCounter<eDate> dayCounter = Thirty360<eDate>(Thirty360<eDate>::BondBasis);
    std::vector<eDate> testStartDates;
    std::vector<eDate> testEndDates;
    serial_type calculated;

    // ISDA - Example 1: End dates do not involve the last day of February
    testStartDates.push_back(DAe::Date(20, August, 2006)); testEndDates.push_back(DAe::Date(20, February, 2007));
    testStartDates.push_back(DAe::Date(20, February, 2007)); testEndDates.push_back(DAe::Date(20, August, 2007));
    testStartDates.push_back(DAe::Date(20, August, 2007)); testEndDates.push_back(DAe::Date(20, February, 2008));
    testStartDates.push_back(DAe::Date(20, February, 2008)); testEndDates.push_back(DAe::Date(20, August, 2008));
    testStartDates.push_back(DAe::Date(20, August, 2008)); testEndDates.push_back(DAe::Date(20, February, 2009));
    testStartDates.push_back(DAe::Date(20, February, 2009)); testEndDates.push_back(DAe::Date(20, August, 2009));

    // ISDA - Example 2: End dates include some end-February dates
    testStartDates.push_back(DAe::Date(31, August, 2006)); testEndDates.push_back(DAe::Date(28, February, 2007));
    testStartDates.push_back(DAe::Date(28, February, 2007)); testEndDates.push_back(DAe::Date(31, August, 2007));
    testStartDates.push_back(DAe::Date(31, August, 2007)); testEndDates.push_back(DAe::Date(29, February, 2008));
    testStartDates.push_back(DAe::Date(29, February, 2008)); testEndDates.push_back(DAe::Date(31, August, 2008));
    testStartDates.push_back(DAe::Date(31, August, 2008)); testEndDates.push_back(DAe::Date(28, February, 2009));
    testStartDates.push_back(DAe::Date(28, February, 2009)); testEndDates.push_back(DAe::Date(31, August, 2009));

    //// ISDA - Example 3: Miscellaneous calculations
    testStartDates.push_back(DAe::Date(31, January, 2006)); testEndDates.push_back(DAe::Date(28, February, 2006));
    testStartDates.push_back(DAe::Date(30, January, 2006)); testEndDates.push_back(DAe::Date(28, February, 2006));
    testStartDates.push_back(DAe::Date(28, February, 2006)); testEndDates.push_back(DAe::Date(3, March, 2006));
    testStartDates.push_back(DAe::Date(14, February, 2006)); testEndDates.push_back(DAe::Date(28, February, 2006));
    testStartDates.push_back(DAe::Date(30, September, 2006)); testEndDates.push_back(DAe::Date(31, October, 2006));
    testStartDates.push_back(DAe::Date(31, October, 2006)); testEndDates.push_back(DAe::Date(28, November, 2006));
    testStartDates.push_back(DAe::Date(31, August, 2007)); testEndDates.push_back(DAe::Date(28, February, 2008));
    testStartDates.push_back(DAe::Date(28, February, 2008)); testEndDates.push_back(DAe::Date(28, August, 2008));
    testStartDates.push_back(DAe::Date(28, February, 2008)); testEndDates.push_back(DAe::Date(30, August, 2008));
    testStartDates.push_back(DAe::Date(28, February, 2008)); testEndDates.push_back(DAe::Date(31, August, 2008));
    testStartDates.push_back(DAe::Date(26, February, 2007)); testEndDates.push_back(DAe::Date(28, February, 2008));
    testStartDates.push_back(DAe::Date(26, February, 2007)); testEndDates.push_back(DAe::Date(29, February, 2008));
    testStartDates.push_back(DAe::Date(29, February, 2008)); testEndDates.push_back(DAe::Date(28, February, 2009));
    testStartDates.push_back(DAe::Date(28, February, 2008)); testEndDates.push_back(DAe::Date(30, March, 2008));
    testStartDates.push_back(DAe::Date(28, February, 2008)); testEndDates.push_back(DAe::Date(31, March, 2008));

    serial_type expected[] = { 180, 180, 180, 180, 180, 180,
                                     178, 183, 179, 182, 178, 183,
                                     28,  28,   5,  14,  30,  28,
                                     178, 180, 182, 183, 362, 363,
                                     359,  32,  33};

    for (Size i = 0; i < testStartDates.size(); i++) {
        calculated = dayCounter.dayCount(testStartDates[i], testEndDates[i]);
        IFe (calculated != expected[i]) {
                BOOST_ERROR("from " << testStartDates[i]
                            << " to " << testEndDates[i] << ":\n"
                            << "    calculated: " << calculated << "\n"
                            << "    expected:   " << expected[i]);
        }
    }
}

TEST_CASE("testThirty360_EurobondBasis", "[DayCounterTest][hide]") {

    BOOST_TEST_MESSAGE("Testing thirty/360 day counter (Eurobond Basis)...");

    // Source: ISDA 2006 Definitions 4.16 (g)
    // 30E/360 (or Eurobond Basis)
    // Based on ICMA (Rule 251) and FBF; this is the version of 30E/360 used by Excel

    DayCounter<eDate> dayCounter = Thirty360<eDate>(Thirty360<eDate>::EurobondBasis);
    std::vector<eDate> testStartDates;
    std::vector<eDate> testEndDates;
    serial_type calculated;

    // ISDA - Example 1: End dates do not involve the last day of February
    testStartDates.push_back(DAe::Date(20, August, 2006)); testEndDates.push_back(DAe::Date(20, February, 2007));
    testStartDates.push_back(DAe::Date(20, February, 2007)); testEndDates.push_back(DAe::Date(20, August, 2007));
    testStartDates.push_back(DAe::Date(20, August, 2007)); testEndDates.push_back(DAe::Date(20, February, 2008));
    testStartDates.push_back(DAe::Date(20, February, 2008)); testEndDates.push_back(DAe::Date(20, August, 2008));
    testStartDates.push_back(DAe::Date(20, August, 2008)); testEndDates.push_back(DAe::Date(20, February, 2009));
    testStartDates.push_back(DAe::Date(20, February, 2009)); testEndDates.push_back(DAe::Date(20, August, 2009));

    //// ISDA - Example 2: End dates include some end-February dates
    testStartDates.push_back(DAe::Date(28, February, 2006)); testEndDates.push_back(DAe::Date(31, August, 2006));
    testStartDates.push_back(DAe::Date(31, August, 2006)); testEndDates.push_back(DAe::Date(28, February, 2007));
    testStartDates.push_back(DAe::Date(28, February, 2007)); testEndDates.push_back(DAe::Date(31, August, 2007));
    testStartDates.push_back(DAe::Date(31, August, 2007)); testEndDates.push_back(DAe::Date(29, February, 2008));
    testStartDates.push_back(DAe::Date(29, February, 2008)); testEndDates.push_back(DAe::Date(31, August, 2008));
    testStartDates.push_back(DAe::Date(31, August, 2008)); testEndDates.push_back(DAe::Date(28, Feb, 2009));
    testStartDates.push_back(DAe::Date(28, February, 2009)); testEndDates.push_back(DAe::Date(31, August, 2009));
    testStartDates.push_back(DAe::Date(31, August, 2009)); testEndDates.push_back(DAe::Date(28, Feb, 2010));
    testStartDates.push_back(DAe::Date(28, February, 2010)); testEndDates.push_back(DAe::Date(31, August, 2010));
    testStartDates.push_back(DAe::Date(31, August, 2010)); testEndDates.push_back(DAe::Date(28, Feb, 2011));
    testStartDates.push_back(DAe::Date(28, February, 2011)); testEndDates.push_back(DAe::Date(31, August, 2011));
    testStartDates.push_back(DAe::Date(31, August, 2011)); testEndDates.push_back(DAe::Date(29, Feb, 2012));

    //// ISDA - Example 3: Miscellaneous calculations
    testStartDates.push_back(DAe::Date(31, January, 2006)); testEndDates.push_back(DAe::Date(28, February, 2006));
    testStartDates.push_back(DAe::Date(30, January, 2006)); testEndDates.push_back(DAe::Date(28, February, 2006));
    testStartDates.push_back(DAe::Date(28, February, 2006)); testEndDates.push_back(DAe::Date(3, March, 2006));
    testStartDates.push_back(DAe::Date(14, February, 2006)); testEndDates.push_back(DAe::Date(28, February, 2006));
    testStartDates.push_back(DAe::Date(30, September, 2006)); testEndDates.push_back(DAe::Date(31, October, 2006));
    testStartDates.push_back(DAe::Date(31, October, 2006)); testEndDates.push_back(DAe::Date(28, November, 2006));
    testStartDates.push_back(DAe::Date(31, August, 2007)); testEndDates.push_back(DAe::Date(28, February, 2008));
    testStartDates.push_back(DAe::Date(28, February, 2008)); testEndDates.push_back(DAe::Date(28, August, 2008));
    testStartDates.push_back(DAe::Date(28, February, 2008)); testEndDates.push_back(DAe::Date(30, August, 2008));
    testStartDates.push_back(DAe::Date(28, February, 2008)); testEndDates.push_back(DAe::Date(31, August, 2008));
    testStartDates.push_back(DAe::Date(26, February, 2007)); testEndDates.push_back(DAe::Date(28, February, 2008));
    testStartDates.push_back(DAe::Date(26, February, 2007)); testEndDates.push_back(DAe::Date(29, February, 2008));
    testStartDates.push_back(DAe::Date(29, February, 2008)); testEndDates.push_back(DAe::Date(28, February, 2009));
    testStartDates.push_back(DAe::Date(28, February, 2008)); testEndDates.push_back(DAe::Date(30, March, 2008));
    testStartDates.push_back(DAe::Date(28, February, 2008)); testEndDates.push_back(DAe::Date(31, March, 2008));

    serial_type expected[] = { 180, 180, 180, 180, 180, 180,
                                     182, 178, 182, 179, 181, 178,
                                     182, 178, 182, 178, 182, 179,
                                     28,  28,   5,  14,  30,  28,
                                     178, 180, 182, 182, 362, 363,
                                     359,  32,  32 };

    for (Size i = 0; i < testStartDates.size(); i++) {
        calculated = dayCounter.dayCount(testStartDates[i], testEndDates[i]);
        IFe (calculated != expected[i]) {
                BOOST_ERROR("from " << testStartDates[i]
                            << " to " << testEndDates[i] << ":\n"
                            << "    calculated: " << calculated << "\n"
                            << "    expected:   " << expected[i]);
        }
    }
}


TEST_CASE("testThirty360_German", "[DayCounterTest][hide]") {
    BOOST_TEST_MESSAGE("Testing 30/360 (German) day counter...");

    Thirty360<eDate> dayCounter(Thirty360<eDate>::German);

    eDate start = DAe::Date(5, February, 2020);
    eDate end = DAe::Date(29, February, 2020);

    serial_type calculated = dayCounter.dayCount(start, end);
    serial_type expected = 25;  // 30 - 5, as 29 is adjusted

    IFe (calculated != expected) {
        BOOST_ERROR("Day count from " << start
                    << " to " << end << ":\n"
                    << "    calculated: " << calculated << "\n"
                    << "    expected:   " << expected);
    }
}


TEST_CASE("testActual365_Canadian", "[DayCounterTest][hide]") {

    BOOST_TEST_MESSAGE("Testing that Actual 365 (Canadian) throws when needed...");

    Actual365Fixed<eDate> dayCounter(Actual365Fixed<eDate>::Canadian);

CHECK_THROWS(

        dayCounter.yearFraction(DAe::Date(10, September, 2018), DAe::Date(10, September, 2019))
);

// reference period shorter than a month
CHECK_THROWS( dayCounter.yearFraction(DAe::Date(10, September, 2018),
                                DAe::Date(12, September, 2018),
                                DAe::Date(10, September, 2018),
                                DAe::Date(15, September, 2018)) 
);
}


TEST_CASE("testIntraday", "[DayCounterTest][hide]") {
#ifdef QL_HIGH_RESOLUTION_DATE

    BOOST_TEST_MESSAGE("Testing intraday behavior of day counter ...");

    const eDate d1(12, February, 2015);
    const eDate d2(14, February, 2015, 12, 34, 17, 1, 230298);

    const Time tol = 100*QL_EPSILON;

    const DayCounter dayCounters[]
        = { ActualActual(), Actual365Fixed(), Actual360() };

    for (Size i=0; i < LENGTH(dayCounters); ++i) {
        const DayCounter dc = dayCounters[i];

        const Time expected = ((12*60 + 34)*60 + 17 + 0.231298)
                             * dc.yearFraction(d1, d1+1)/86400
                             + dc.yearFraction(d1, d1+2);

        BOOST_CHECK_MESSAGE(
            std::fabs(dc.yearFraction(d1, d2) - expected) < tol,
            "can not reproduce result for day counter " << dc.name());

        BOOST_CHECK_MESSAGE(
            std::fabs(dc.yearFraction(d2, d1) + expected) < tol,
            "can not reproduce result for day counter " << dc.name());
    }
#endif
}


