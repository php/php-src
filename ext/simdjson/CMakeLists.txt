cmake_minimum_required(VERSION 3.8)
project(simdjson)

add_compile_definitions(HAVE_SIMDJSON)

set(SOURCE_FILES
    php_simdjson.h
    simdjson.cpp
    src/bindings.h
    src/bindings.cpp
    src/simdjson.h
    src/simdjson.cpp
)

execute_process (
        COMMAND php-config --include-dir
        OUTPUT_VARIABLE PHP_SOURCE
)
string(REGEX REPLACE "\n$" "" PHP_SOURCE "${PHP_SOURCE}")

message("Using source directory: ${PHP_SOURCE}")

include_directories(
    "${PHP_SOURCE}"
    "${PHP_SOURCE}/main"
    "${PHP_SOURCE}/Zend"
    "${PHP_SOURCE}/TSRM"
    "${PROJECT_SOURCE_DIR}"
)

add_custom_target(configure
        COMMAND phpize && ./configure
        DEPENDS ${SOURCE_FILES}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_library(___ EXCLUDE_FROM_ALL ${SOURCE_FILES})
