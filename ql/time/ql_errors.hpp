#pragma once

#include <cpp_rutils/require.h>

#define QL_FAIL(...) MREQUIRE1(false, __VA_ARGS__)

#define QL_REQUIRE MREQUIRE

#define QL_ASSERT MREQUIRE

#define QL_ENSURE(condition, ...) \
    QL_REQUIRE((condition),__VA_ARGS__)