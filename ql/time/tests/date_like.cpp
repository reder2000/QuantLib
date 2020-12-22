#include <catch.hpp>
#include "../date_like.hpp"
#include <ql/time/calendars/target.hpp>
#include <ql/time/calendars/unitedkingdom.hpp>
#include <ql/time/daycounters/actual360.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include <ql/time/daycounters/actualactual.hpp>
#include <ql/time/daycounters/business252.hpp>

#include "pseudo_dates.h"


void f() {
    { QuantLib::TARGET<pseudoDate> c; }
    { QuantLib::UnitedKingdom<pseudoDate> c; }
    { QuantLib::Actual360<pseudoDate> c; }
    { QuantLib::Actual365Fixed<pseudoDate> c; }
    { QuantLib::ActualActual<pseudoDate> c; }
    { QuantLib::Business252<pseudoDate> c; }
}
