--TEST--
Tests for DateTime[Immutable]::createFromTimestamp
--INI--
date.timezone=Europe/London
--FILE--
<?php

class MyDateTime extends DateTime {};
class MyDateTimeImmutable extends DateTimeImmutable {};

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
    PHP_INT_MAX + 1024.0,
    PHP_INT_MIN - 1025.0,
    NAN,
    +INF,
    -INF
);

foreach ($timestamps as $ts) {
    echo 'DateTime::createFromTimestamp(' . var_export($ts, true) . '): ';
    try {
        var_dump(DateTime::createFromTimestamp($ts));
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
DateTime::createFromTimestamp(1696883232): object(DateTime)#%d (3) {
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
DateTime::createFromTimestamp(-1696883232): object(DateTime)#%d (3) {
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
DateTime::createFromTimestamp(1696883232.013981): object(DateTime)#%d (3) {
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
DateTime::createFromTimestamp(-1696883232.013981): object(DateTime)#%d (3) {
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
DateTime::createFromTimestamp(0.123456): object(DateTime)#%d (3) {
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
DateTime::createFromTimestamp(-0.123456): object(DateTime)#%d (3) {
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
DateTime::createFromTimestamp(0): object(DateTime)#%d (3) {
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
DateTime::createFromTimestamp(0.0): object(DateTime)#%d (3) {
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
DateTime::createFromTimestamp(-0.0): object(DateTime)#%d (3) {
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
DateTime::createFromTimestamp(%f): DateRangeError: DateTime::createFromTimestamp(): Argument #1 ($timestamp) must be a finite number between %i and %i.999999, %f given
DateTimeImmutable::createFromTimestamp(%f): DateRangeError: DateTimeImmutable::createFromTimestamp(): Argument #1 ($timestamp) must be a finite number between %i and %i.999999, %f given
DateTime::createFromTimestamp(%f): DateRangeError: DateTime::createFromTimestamp(): Argument #1 ($timestamp) must be a finite number between %i and %i.999999, %f given
DateTimeImmutable::createFromTimestamp(%f): DateRangeError: DateTimeImmutable::createFromTimestamp(): Argument #1 ($timestamp) must be a finite number between %i and %i.999999, %f given
DateTime::createFromTimestamp(NAN): DateRangeError: DateTime::createFromTimestamp(): Argument #1 ($timestamp) must be a finite number between %i and %i.999999, NAN given
DateTimeImmutable::createFromTimestamp(NAN): DateRangeError: DateTimeImmutable::createFromTimestamp(): Argument #1 ($timestamp) must be a finite number between %i and %i.999999, NAN given
DateTime::createFromTimestamp(INF): DateRangeError: DateTime::createFromTimestamp(): Argument #1 ($timestamp) must be a finite number between %i and %i.999999, INF given
DateTimeImmutable::createFromTimestamp(INF): DateRangeError: DateTimeImmutable::createFromTimestamp(): Argument #1 ($timestamp) must be a finite number between %i and %i.999999, INF given
DateTime::createFromTimestamp(-INF): DateRangeError: DateTime::createFromTimestamp(): Argument #1 ($timestamp) must be a finite number between %i and %i.999999, -INF given
DateTimeImmutable::createFromTimestamp(-INF): DateRangeError: DateTimeImmutable::createFromTimestamp(): Argument #1 ($timestamp) must be a finite number between %i and %i.999999, -INF given
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
