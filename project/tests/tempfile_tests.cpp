#include "catch.hpp"
#include "../tempfile.h"

TEST_CASE("test1", "[correctness]") {
    int a = 100;
    REQUIRE(foobar(a) == 5);
}