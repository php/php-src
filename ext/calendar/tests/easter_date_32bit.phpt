--TEST--
Test easter_date() year range on 32bit systems
--SKIPIF--
<?php
if (PHP_SYS_SIZE != 4) die("skip 32-bit only");
// Some 32-bit platforms have a 64-bit time_t (e.g. armhf with glibc t64, musl)
try { easter_date(2040); die("skip 32-bit timestamps only"); } catch (ValueError) {}
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
foreach ([PHP_INT_MIN, 1, 1969, 1970, 2000, 2001, 2002, 2037, 2038, PHP_INT_MAX] as $year) {
    echo $year, ': ';
    try {
        echo date('Y-m-d', easter_date($year)), PHP_EOL;
    } catch (ValueError $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}
?>
--EXPECTF--
%i: ValueError: easter_date(): Argument #1 ($year) must be between 1970 and 2037
1: ValueError: easter_date(): Argument #1 ($year) must be between 1970 and 2037
1969: ValueError: easter_date(): Argument #1 ($year) must be between 1970 and 2037
1970: 1970-03-29
2000: 2000-04-23
2001: 2001-04-15
2002: 2002-03-31
2037: 2037-04-05
2038: ValueError: easter_date(): Argument #1 ($year) must be between 1970 and 2037
%d: ValueError: easter_date(): Argument #1 ($year) must be between 1970 and 2037
