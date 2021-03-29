cmake_minimum_required(VERSION 3.19)
project("QuantLib" VERSION 1.9.2
             DESCRIPTION "adapted quantlib with drop-in date type"
             HOMEPAGE_URL "https://github.com/reder2000/QuantLib")


add_subdirectory(ql/time)

enable_testing()