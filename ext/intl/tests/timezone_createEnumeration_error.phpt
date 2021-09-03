--TEST--
IntlTimeZone::createEnumeration(): errors
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::createEnumeration(array()));
?>
--EXPECTF--
Warning: IntlTimeZone::createEnumeration(): intltz_create_enumeration: invalid argument type in %s on line %d
bool(false)
