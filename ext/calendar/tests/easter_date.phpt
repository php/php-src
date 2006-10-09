--TEST--
easter_date()
--INI--
date.timezone=UTC
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
echo date("Y-m-d", easter_date(2000))."\n";       
echo date("Y-m-d", easter_date(2001))."\n";      
echo date("Y-m-d", easter_date(2002))."\n";      
echo date("Y-m-d", easter_date(1492))."\n";
?>
--EXPECTF--
2000-04-22
2001-04-14
2002-03-30

Warning: easter_date(): This function is only valid for years between 1970 and 2037 inclusive in %s on line %d
1970-01-01
