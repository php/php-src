--TEST--
IntlGregorianCalendar::getGregorianChange(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(intlgregcal_get_gregorian_change(1));

?>
--EXPECTF--
Fatal error: Uncaught TypeError: intlgregcal_get_gregorian_change(): Argument #1 ($calendar) must be of type IntlGregorianCalendar, int given in %s:%d
Stack trace:
#0 %s(%d): intlgregcal_get_gregorian_change(1)
#1 {main}
  thrown in %s on line %d
