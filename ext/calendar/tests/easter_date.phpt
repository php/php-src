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
echo date("Y-m-d", easter_date(1492))."\n";
?>
--EXPECTF--
2000-04-23
2001-04-15
2002-03-31

Warning: easter_date(): This function is only valid for years between 1970 and 2037 inclusive in %s on line %d
1970-01-01
