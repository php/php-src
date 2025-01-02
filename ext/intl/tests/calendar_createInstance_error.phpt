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

try {
	intlcal_create_instance(new X, NULL);
} catch (IntlException $e) {
	echo $e->getMessage();
}
?>
--EXPECT--
intlcal_create_instance: passed IntlTimeZone is not properly constructed
