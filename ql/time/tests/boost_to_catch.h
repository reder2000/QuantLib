#pragma once

#define BOOST_TEST_MESSAGE INFO
#define BOOST_FAIL(...) 
#define BOOST_ERROR(...)
#define IF(a) CHECK_FALSE(a)
#define IIF(a) CHECK_FALSE((a)); if(a)
#define elseIF(a) else if ([=](){IF((a));},a)

//TEST_CASE("ecbDates", "[DateTest][hide]") {
