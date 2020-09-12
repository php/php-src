--TEST--
DatePeriod: Test wrong __construct parameter
--CREDITS--
Havard Eide <nucleuz@gmail.com>
#PHPTestFest2009 Norway 2009-06-09 \o/
--INI--
date.timezone=UTC
--FILE--
<?php
new DatePeriod();
?>
--EXPECTF--
Fatal error: Uncaught ArgumentCountError: DatePeriod::__construct() expects at least 1 parameter, 0 given in %s:%d
Stack trace:
#0 %s(%d): DatePeriod->__construct()
#1 {main}
  thrown in %s on line %d
