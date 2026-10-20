--TEST--
GH-23915 (easter_date() reports inconsistent year ranges in errors and returns -1 for years after 3000 on Windows)
--INI--
date.timezone=UTC
--ENV--
TZ=UTC
--EXTENSIONS--
calendar
--FILE--
<?php
putenv('TZ=UTC');
foreach ([PHP_INT_MIN, 0, 1, 1969, 1970, PHP_INT_MAX] as $year) {
    echo $year, ': ';
    try {
        echo date('Y-m-d', easter_date($year)), "\n";
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--EXPECTF--
%i: ValueError: easter_date(): Argument #1 ($year) must be between 1970 and %d
0: ValueError: easter_date(): Argument #1 ($year) must be between 1970 and %d
1: ValueError: easter_date(): Argument #1 ($year) must be between 1970 and %d
1969: ValueError: easter_date(): Argument #1 ($year) must be between 1970 and %d
1970: 1970-03-29
%d: ValueError: easter_date(): Argument #1 ($year) must be between 1970 and %d
