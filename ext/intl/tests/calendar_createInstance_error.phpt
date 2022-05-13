--TEST--
IntlCalendar::createInstance: bad arguments
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

class X extends IntlTimeZone {
function __construct() {}
}

var_dump(intlcal_create_instance(new X, NULL));
?>
--EXPECTF--
Warning: intlcal_create_instance(): intlcal_create_instance: passed IntlTimeZone is not properly constructed in %s on line %d
NULL
