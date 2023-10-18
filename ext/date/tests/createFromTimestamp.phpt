--TEST--
Tests for DateTime[Immutable]::createFromTimestamp & date_create_[immutable_]from_timestamp
--INI--
date.timezone=Europe/London
--FILE--
<?php

class MyDateTime extends DateTime {};
class MyDateTimeImmutable extends DateTimeImmutable {};

define('MAX_32BIT', 2147483647);
define('MIN_32BIT', -2147483648);

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
    PHP_INT_MAX + 1024.0,
    PHP_INT_MIN - 1025.0,
    NAN,
    +INF,
    -INF
);

foreach ($timestamps as $ts) {
    echo 'date_create_from_timestamp(' . var_export($ts, true) . '): ';
    try {
        var_dump(date_create_from_timestamp($ts));
    } catch (Throwable $e) {
        echo get_class($e) . ': ' . $e->getMessage() . "\n";
    }

    echo 'DateTime::createFromTimestamp(' . var_export($ts, true) . '): ';
    try {
        var_dump(DateTime::createFromTimestamp($ts));
    } catch (Throwable $e) {
        echo get_class($e) . ': ' . $e->getMessage() . "\n";
    }

    echo 'date_create_immutable_from_timestamp(' . var_export($ts, true) . '): ';
    try {
        var_dump(date_create_immutable_from_timestamp($ts));
    } catch (Throwable $e) {
        echo get_class($e) . ': ' . $e->getMessage() . "\n";
    }

    echo 'DateTimeImmutable::createFromTimestamp(' . var_export($ts, true) . '): ';
    try {
        var_dump(DateTimeImmutable::createFromTimestamp($ts));
    } catch (Throwable $e) {
        echo get_class($e) . ': ' . $e->getMessage() . "\n";
    }
}

echo 'MyDateTime::createFromTimestamp(' . var_export(0, true) . '): ';
try {
    var_dump(MyDateTime::createFromTimestamp(0));
} catch (Throwable $e) {
    echo get_class($e) . ': ' . $e->getMessage() . "\n";
}

echo 'MyDateTimeImmutable::createFromTimestamp(' . var_export(0, true) . '): ';
try {
    var_dump(MyDateTimeImmutable::createFromTimestamp(0));
} catch (Throwable $e) {
    echo get_class($e) . ': ' . $e->getMessage() . "\n";
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
date_create_from_timestamp(9.223372036854776E+18): DateRangeError: Seconds must be a finite number between %i and %i, 9.22337e+18 given
DateTime::createFromTimestamp(9.223372036854776E+18): DateRangeError: Seconds must be a finite number between %i and %i, 9.22337e+18 given
date_create_immutable_from_timestamp(9.223372036854776E+18): DateRangeError: Seconds must be a finite number between %i and %i, 9.22337e+18 given
DateTimeImmutable::createFromTimestamp(9.223372036854776E+18): DateRangeError: Seconds must be a finite number between %i and %i, 9.22337e+18 given
date_create_from_timestamp(-9.223372036854778E+18): DateRangeError: Seconds must be a finite number between %i and %i, -9.22337e+18 given
DateTime::createFromTimestamp(-9.223372036854778E+18): DateRangeError: Seconds must be a finite number between %i and %i, -9.22337e+18 given
date_create_immutable_from_timestamp(-9.223372036854778E+18): DateRangeError: Seconds must be a finite number between %i and %i, -9.22337e+18 given
DateTimeImmutable::createFromTimestamp(-9.223372036854778E+18): DateRangeError: Seconds must be a finite number between %i and %i, -9.22337e+18 given
date_create_from_timestamp(NAN): DateRangeError: Seconds must be a finite number between %i and %i, NAN given
DateTime::createFromTimestamp(NAN): DateRangeError: Seconds must be a finite number between %i and %i, NAN given
date_create_immutable_from_timestamp(NAN): DateRangeError: Seconds must be a finite number between %i and %i, NAN given
DateTimeImmutable::createFromTimestamp(NAN): DateRangeError: Seconds must be a finite number between %i and %i, NAN given
date_create_from_timestamp(INF): DateRangeError: Seconds must be a finite number between %i and %i, INF given
DateTime::createFromTimestamp(INF): DateRangeError: Seconds must be a finite number between %i and %i, INF given
date_create_immutable_from_timestamp(INF): DateRangeError: Seconds must be a finite number between %i and %i, INF given
DateTimeImmutable::createFromTimestamp(INF): DateRangeError: Seconds must be a finite number between %i and %i, INF given
date_create_from_timestamp(-INF): DateRangeError: Seconds must be a finite number between %i and %i, -INF given
DateTime::createFromTimestamp(-INF): DateRangeError: Seconds must be a finite number between %i and %i, -INF given
date_create_immutable_from_timestamp(-INF): DateRangeError: Seconds must be a finite number between %i and %i, -INF given
DateTimeImmutable::createFromTimestamp(-INF): DateRangeError: Seconds must be a finite number between %i and %i, -INF given
MyDateTime::createFromTimestamp(0): object(MyDateTime)#%d (3) {
  ["date"]=>
  string(26) "1970-01-01 00:00:00.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
MyDateTimeImmutable::createFromTimestamp(0): object(MyDateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "1970-01-01 00:00:00.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
