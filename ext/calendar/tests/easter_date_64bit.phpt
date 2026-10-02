--TEST--
Test easter_date() year range on 64bit systems
--SKIPIF--
<?php
if (PHP_SYS_SIZE != 8) die("skip 64-bit only");
if (PHP_OS_FAMILY === "Windows") die("skip not for Windows, see easter_date_64bit_windows.phpt");
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
foreach ([PHP_INT_MIN, 1, 1969, 1970, 2000, 2001, 2002, 2037, 2038, 2045, 2046, 2047, 1999999999, 2000000000, 2000000001, PHP_INT_MAX] as $year) {
    echo $year, ': ';
    try {
        echo date('Y-m-d', easter_date($year)), PHP_EOL;
    } catch (ValueError $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}
?>
--EXPECT--
-9223372036854775808: ValueError: easter_date(): Argument #1 ($year) must be between 1970 and 2000000000
1: ValueError: easter_date(): Argument #1 ($year) must be between 1970 and 2000000000
1969: ValueError: easter_date(): Argument #1 ($year) must be between 1970 and 2000000000
1970: 1970-03-29
2000: 2000-04-23
2001: 2001-04-15
2002: 2002-03-31
2037: 2037-04-05
2038: 2038-04-25
2045: 2045-04-09
2046: 2046-03-25
2047: 2047-04-14
1999999999: 1999999999-04-04
2000000000: 2000000000-04-23
2000000001: ValueError: easter_date(): Argument #1 ($year) must be between 1970 and 2000000000
9223372036854775807: ValueError: easter_date(): Argument #1 ($year) must be between 1970 and 2000000000
