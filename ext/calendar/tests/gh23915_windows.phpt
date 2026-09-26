--TEST--
GH-23915 (easter_date() reports inconsistent year ranges in errors and returns -1 for years after 3000 on Windows)
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
foreach ([3000, 3001, 293274701009] as $year) {
    try {
        echo date('Y-m-d', easter_date($year)), "\n";
    } catch (ValueError $ex) {
        echo "{$ex->getMessage()}\n";
    }
}
?>
--EXPECT--
3000-04-13
easter_date(): Argument #1 ($year) must be between 1970 and 3000
easter_date(): Argument #1 ($year) must be between 1970 and 3000
