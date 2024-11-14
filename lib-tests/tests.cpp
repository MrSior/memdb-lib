#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

unsigned int factorial( unsigned int number ) {
    return number <= 1 ? number : factorial(number-1)*number;
}

TEST_CASE( "Factorials are computed", "[factorial]" ) {
    REQUIRE( factorial(1) == 1 );
    REQUIRE( factorial(2) == 2 );
    REQUIRE( factorial(3) == 6 );
    REQUIRE( factorial(10) == 3628800 );

    // This find a bug in the factorial implementation,
    // try uncommenting it.
    // REQUIRE( factorial(0) == 1 );

    BENCHMARK("fibonacci 25") {
                                  return factorial(25);
                              };
}

TEST_CASE("Example test with tag", "[mytag]") {
    BENCHMARK("fibonacci 25") {
                                  return factorial(25);
                              };
}