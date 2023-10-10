--TEST--
Tests for DateTime[Immutable]::createFromTimestamp & date_create_[immutable_]_from_timestamp
--INI--
date.timezone=Europe/London
--FILE--
<?php

define('MAX_32BIT', 2147483647);
define('MIN_32BIT', -2147483648);
define('UMAX_64BIT', 18446744073709551616.0);

$int = 1696883232;
$float = 1696883232.013981;

$timestamps = array(
    1696883232,
    -1696883232,
    1696883232.013981,
    -1696883232.013981,
    0.123456,
    -0.123456,
    0,
    0.0,
    -0.0,
    MAX_32BIT,
    MIN_32BIT,
    UMAX_64BIT,
    -UMAX_64BIT,
    NAN,
    +INF,
    -INF
);

foreach ($timestamps as $ts) {
    echo 'date_create_from_timestamp(' . var_export($ts, true) . '): ';
    var_dump(date_create_from_timestamp($ts));

    echo 'DateTime::createFromTimestamp(' . var_export($ts, true) . '): ';
    var_dump(DateTime::createFromTimestamp($ts));

    echo 'date_create_immutable_from_timestamp(' . var_export($ts, true) . '): ';
    var_dump(date_create_immutable_from_timestamp($ts));

    echo 'DateTimeImmutable::createFromTimestamp(' . var_export($ts, true) . '): ';
    var_dump(DateTimeImmutable::createFromTimestamp($ts));
}

?>
--EXPECTF--
date_create_from_timestamp(1696883232): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2023-10-09 20:27:12.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTime::createFromTimestamp(1696883232): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2023-10-09 20:27:12.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_immutable_from_timestamp(1696883232): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2023-10-09 20:27:12.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTimeImmutable::createFromTimestamp(1696883232): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2023-10-09 20:27:12.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_from_timestamp(-1696883232): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "1916-03-25 03:32:48.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTime::createFromTimestamp(-1696883232): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "1916-03-25 03:32:48.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_immutable_from_timestamp(-1696883232): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "1916-03-25 03:32:48.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTimeImmutable::createFromTimestamp(-1696883232): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "1916-03-25 03:32:48.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_from_timestamp(1696883232.013981): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2023-10-09 20:27:12.013981"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTime::createFromTimestamp(1696883232.013981): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2023-10-09 20:27:12.013981"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_immutable_from_timestamp(1696883232.013981): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2023-10-09 20:27:12.013981"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTimeImmutable::createFromTimestamp(1696883232.013981): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2023-10-09 20:27:12.013981"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_from_timestamp(-1696883232.013981): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "1916-03-25 03:32:47.986019"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTime::createFromTimestamp(-1696883232.013981): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "1916-03-25 03:32:47.986019"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_immutable_from_timestamp(-1696883232.013981): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "1916-03-25 03:32:47.986019"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTimeImmutable::createFromTimestamp(-1696883232.013981): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "1916-03-25 03:32:47.986019"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_from_timestamp(0.123456): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "1970-01-01 00:00:00.123456"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTime::createFromTimestamp(0.123456): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "1970-01-01 00:00:00.123456"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_immutable_from_timestamp(0.123456): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "1970-01-01 00:00:00.123456"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTimeImmutable::createFromTimestamp(0.123456): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "1970-01-01 00:00:00.123456"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_from_timestamp(-0.123456): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "1969-12-31 23:59:59.876544"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTime::createFromTimestamp(-0.123456): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "1969-12-31 23:59:59.876544"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_immutable_from_timestamp(-0.123456): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "1969-12-31 23:59:59.876544"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTimeImmutable::createFromTimestamp(-0.123456): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "1969-12-31 23:59:59.876544"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_from_timestamp(0): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "1970-01-01 00:00:00.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTime::createFromTimestamp(0): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "1970-01-01 00:00:00.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_immutable_from_timestamp(0): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "1970-01-01 00:00:00.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTimeImmutable::createFromTimestamp(0): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "1970-01-01 00:00:00.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_from_timestamp(0.0): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "1970-01-01 00:00:00.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTime::createFromTimestamp(0.0): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "1970-01-01 00:00:00.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_immutable_from_timestamp(0.0): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "1970-01-01 00:00:00.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTimeImmutable::createFromTimestamp(0.0): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "1970-01-01 00:00:00.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_from_timestamp(-0.0): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "1970-01-01 00:00:00.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTime::createFromTimestamp(-0.0): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "1970-01-01 00:00:00.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_immutable_from_timestamp(-0.0): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "1970-01-01 00:00:00.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTimeImmutable::createFromTimestamp(-0.0): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "1970-01-01 00:00:00.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_from_timestamp(2147483647): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2038-01-19 03:14:07.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTime::createFromTimestamp(2147483647): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2038-01-19 03:14:07.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_immutable_from_timestamp(2147483647): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2038-01-19 03:14:07.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTimeImmutable::createFromTimestamp(2147483647): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2038-01-19 03:14:07.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_from_timestamp(-2147483648): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "1901-12-13 20:45:52.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTime::createFromTimestamp(-2147483648): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "1901-12-13 20:45:52.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_immutable_from_timestamp(-2147483648): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "1901-12-13 20:45:52.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTimeImmutable::createFromTimestamp(-2147483648): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "1901-12-13 20:45:52.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
date_create_from_timestamp(1.8446744073709552E+19): bool(false)
DateTime::createFromTimestamp(1.8446744073709552E+19): bool(false)
date_create_immutable_from_timestamp(1.8446744073709552E+19): bool(false)
DateTimeImmutable::createFromTimestamp(1.8446744073709552E+19): bool(false)
date_create_from_timestamp(-1.8446744073709552E+19): bool(false)
DateTime::createFromTimestamp(-1.8446744073709552E+19): bool(false)
date_create_immutable_from_timestamp(-1.8446744073709552E+19): bool(false)
DateTimeImmutable::createFromTimestamp(-1.8446744073709552E+19): bool(false)
date_create_from_timestamp(NAN): bool(false)
DateTime::createFromTimestamp(NAN): bool(false)
date_create_immutable_from_timestamp(NAN): bool(false)
DateTimeImmutable::createFromTimestamp(NAN): bool(false)
date_create_from_timestamp(INF): bool(false)
DateTime::createFromTimestamp(INF): bool(false)
date_create_immutable_from_timestamp(INF): bool(false)
DateTimeImmutable::createFromTimestamp(INF): bool(false)
date_create_from_timestamp(-INF): bool(false)
DateTime::createFromTimestamp(-INF): bool(false)
date_create_immutable_from_timestamp(-INF): bool(false)
DateTimeImmutable::createFromTimestamp(-INF): bool(false)
