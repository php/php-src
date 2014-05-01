--TEST--
IntlTimeZone::getCanonicalID(): second argument
--SKIPIF--
<?php if (!extension_loaded('intl')) die('skip intl extension not enabled'); ?>
<?php if (version_compare(INTL_ICU_VERSION, '53.1') < 0) die('skip for ICU >= 53.1'); ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::getCanonicalID('Portugal', $isSystemId));
var_dump($isSystemId);

/* A valid custom time zone ID has the following syntax: GMT[+|-]hh[[:]mm] */
var_dump(IntlTimeZone::getCanonicalID('GMT +01:25', $isSystemId));
var_dump($isSystemId);

?>
==DONE==
--EXPECTF--
string(13) "Europe/Lisbon"
bool(true)

Warning: IntlTimeZone::getCanonicalID(): intltz_get_canonical_id: error obtaining canonical ID in %stimezone_getCanonicalID_variant1_2.php on line %d
bool(false)
bool(true)
==DONE==
