#include <catch.hpp>
#include "../date_like.hpp"
#include <ql/time/calendars/target.hpp>
#include <ql/time/calendars/unitedkingdom.hpp>
#include <ql/time/calendars/thailand.hpp>
#include <ql/time/daycounters/actual360.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include <ql/time/daycounters/actualactual.hpp>
#include <ql/time/daycounters/business252.hpp>


#include "pseudo_dates.h"

using eDate = QuantLibDate;

void f() {
    { QuantLib::TARGET<eDate> c; }
    { QuantLib::UnitedKingdom<eDate> c; }
    { QuantLib::Actual360<eDate> c; }
    { QuantLib::Actual365Fixed<eDate> c; }
    { QuantLib::ActualActual<eDate> c; }
    { QuantLib::Business252<eDate> c; }
}
