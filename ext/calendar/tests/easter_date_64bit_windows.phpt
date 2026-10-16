--TEST--
Test easter_date() year range on 64bit Windows systems
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip 64-bit only");
if (PHP_OS_FAMILY !== "Windows") die("skip Windows only");
?>
--INI--
date.timezone=UTC
--ENV--
TZ=UTC
--EXTENSIONS--
calendar
--FILE--
<?php
putenv('TZ=UTC');
// mktime() on Windows only goes up to the year 3000
foreach ([PHP_INT_MIN, 1, 1969, 1970, 2000, 2037, 2038, 2999, 3000, 3001, PHP_INT_MAX] as $year) {
    echo $year, ': ';
    try {
        echo date('Y-m-d', easter_date($year)), PHP_EOL;
    } catch (ValueError $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}
?>
--EXPECT--
-9223372036854775808: ValueError: easter_date(): Argument #1 ($year) must be between 1970 and 3000
1: ValueError: easter_date(): Argument #1 ($year) must be between 1970 and 3000
1969: ValueError: easter_date(): Argument #1 ($year) must be between 1970 and 3000
1970: 1970-03-29
2000: 2000-04-23
2037: 2037-04-05
2038: 2038-04-25
2999: 2999-03-24
3000: 3000-04-13
3001: ValueError: easter_date(): Argument #1 ($year) must be between 1970 and 3000
9223372036854775807: ValueError: easter_date(): Argument #1 ($year) must be between 1970 and 3000
