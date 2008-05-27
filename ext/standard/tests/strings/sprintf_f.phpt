--TEST--
sprintf %f
--FILE--
<?php

var_dump(sprintf("%3.2f", 1.2));
var_dump(sprintf("%-3.2f", 1.2));
var_dump(sprintf("%03.2f", 1.2));
var_dump(sprintf("%-03.2f", 1.2));
echo "\n";
var_dump(sprintf("%5.2f", 3.4));
var_dump(sprintf("%-5.2f", 3.4));
var_dump(sprintf("%05.2f", 3.4));
var_dump(sprintf("%-05.2f", 3.4));
echo "\n";
var_dump(sprintf("%7.2f", -5.6));
var_dump(sprintf("%-7.2f", -5.6));
var_dump(sprintf("%07.2f", -5.6));
var_dump(sprintf("%-07.2f", -5.6));
echo "\n";
var_dump(sprintf("%3.4f", 1.2345678e99));

?>
--EXPECTF--
unicode(4) "1.20"
unicode(4) "1.20"
unicode(4) "1.20"
unicode(4) "1.20"

unicode(5) " 3.40"
unicode(5) "3.40 "
unicode(5) "03.40"
unicode(5) "3.400"

unicode(7) "  -5.60"
unicode(7) "-5.60  "
unicode(7) "-005.60"
unicode(7) "-5.6000"

unicode(105) "1234567%d.0000"
