--TEST--
cal_days_in_month: test invalid parameter
--CREDITS--
Havard Eide <nucleuz@gmail.com>
#PHPTestFest2009 Norway 2009-06-09 \o/
--SKIPIF--
<?php if (!extension_loaded("calendar")) { echo "skip extension not available"; } ?>
--INI--
date.timezone=UTC
--FILE--
<?php
echo cal_days_in_month(CAL_GREGORIAN,0, 2009);
?>
--EXPECTF--
Warning: cal_days_in_month(): invalid date. in %s on line %d

