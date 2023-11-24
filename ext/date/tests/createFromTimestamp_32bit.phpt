--TEST--
Tests for DateTime[Immutable]::createFromTimestamp 32bit variant
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--INI--
date.timezone=Europe/London
--FILE--
<?php

$timestamps = array(
    PHP_INT_MAX,
    PHP_INT_MIN,
    PHP_INT_MAX + 0.5,
    PHP_INT_MIN + 0.5,
    PHP_INT_MAX - 0.5,
    PHP_INT_MIN - 0.5,
    PHP_INT_MAX + 1024.0,
    PHP_INT_MIN - 1025.0,
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

?>
--EXPECTF--
DateTime::createFromTimestamp(2147483647): object(DateTime)#%d (3) {
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
DateTime::createFromTimestamp(-2147483647-1): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "1901-12-13 20:45:52.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTimeImmutable::createFromTimestamp(-2147483647-1): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "1901-12-13 20:45:52.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTime::createFromTimestamp(2147483647.5): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2038-01-19 03:14:07.500000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTimeImmutable::createFromTimestamp(2147483647.5): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2038-01-19 03:14:07.500000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTime::createFromTimestamp(-2147483647.5): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "1901-12-13 20:45:52.500000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTimeImmutable::createFromTimestamp(-2147483647.5): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "1901-12-13 20:45:52.500000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTime::createFromTimestamp(2147483646.5): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2038-01-19 03:14:06.500000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTimeImmutable::createFromTimestamp(2147483646.5): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2038-01-19 03:14:06.500000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTime::createFromTimestamp(-2147483648.5): DateRangeError: Seconds must be a finite number between -2147483648 and 2147483647, -2.14748e+9 given
DateTimeImmutable::createFromTimestamp(-2147483648.5): DateRangeError: Seconds must be a finite number between -2147483648 and 2147483647, -2.14748e+9 given
DateTime::createFromTimestamp(2147484671.0): DateRangeError: Seconds must be a finite number between -2147483648 and 2147483647, 2.14748e+9 given
DateTimeImmutable::createFromTimestamp(2147484671.0): DateRangeError: Seconds must be a finite number between -2147483648 and 2147483647, 2.14748e+9 given
DateTime::createFromTimestamp(-2147484673.0): DateRangeError: Seconds must be a finite number between -2147483648 and 2147483647, -2.14748e+9 given
DateTimeImmutable::createFromTimestamp(-2147484673.0): DateRangeError: Seconds must be a finite number between -2147483648 and 2147483647, -2.14748e+9 given

