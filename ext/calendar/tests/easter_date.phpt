--TEST--
easter_date()
--INI--
date.timezone=UTC
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
putenv('TZ=UTC');
echo date("Y-m-d", easter_date(2000))."\n";
echo date("Y-m-d", easter_date(2001))."\n";
echo date("Y-m-d", easter_date(2002))."\n";
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
This function is only valid for years between 1970 and 2037 inclusive
