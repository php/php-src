--TEST--
IntlTimeZone::getCanonicalID(): second argument
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::getCanonicalID('Portugal', $isSystemId));
var_dump($isSystemId);

/* A valid custom time zone ID has the following syntax: GMT[+|-]hh[[:]mm] */
var_dump(IntlTimeZone::getCanonicalID('GMT +01:25', $isSystemId));
var_dump($isSystemId);

?>
--EXPECTF--
string(13) "Europe/Lisbon"
bool(true)

Warning: IntlTimeZone::getCanonicalID(): error obtaining canonical ID in %stimezone_getCanonicalID_variant1_2.php on line %d
bool(false)
bool(true)
