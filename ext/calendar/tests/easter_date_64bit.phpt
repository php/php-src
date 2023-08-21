--TEST--
Test easter_date() on 64bit systems
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip 64-bit only"); ?>
--INI--
date.timezone=UTC
--ENV--
TZ=UTC
--EXTENSIONS--
calendar
--FILE--
<?php
putenv('TZ=UTC');
echo date("Y-m-d", easter_date(2000))."\n";
echo date("Y-m-d", easter_date(2001))."\n";
echo date("Y-m-d", easter_date(2002))."\n";
echo date("Y-m-d", easter_date(2045))."\n";
echo date("Y-m-d", easter_date(2046))."\n";
echo date("Y-m-d", easter_date(2047))."\n";
try {
    easter_date(1492);
} catch (ValueError $ex) {
    echo "{$ex->getMessage()}\n";
}
?>
--EXPECT--
2000-04-23
2001-04-15
2002-03-31
2045-04-09
2046-03-25
2047-04-14
easter_date(): Argument #1 ($year) must be a year after 1970 (inclusive)
