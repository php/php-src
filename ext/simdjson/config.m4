PHP_ARG_ENABLE([simdjson],
  [whether to enable simdjson],
  [AS_HELP_STRING([--enable-simdjson],
    [Enable simdjson support])])

if test "$PHP_SIMDJSON" != "no"; then

  CXXFLAGS=" -march=native -O2 -std=c++17 "
  PHP_REQUIRE_CXX()

  PHP_SUBST(SIMDJSON_SHARED_LIBADD)
  PHP_ADD_LIBRARY(stdc++, 1, SIMDJSON_SHARED_LIBADD)

  AC_DEFINE(HAVE_SIMDJSON, 1, [whether simdjson is enabled])
  PHP_NEW_EXTENSION(simdjson,
      simdjson.cpp                        \
      src/bindings.cpp                    \
      src/simdjson.cpp,
    $ext_shared)

  PHP_ADD_MAKEFILE_FRAGMENT
  PHP_INSTALL_HEADERS([ext/simdjson], [php_simdjson.h])
fi
