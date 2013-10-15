--TEST--
IntlTimeZone::getCanonicalID(): second argument
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::getCanonicalID('Portugal', $isSystemId));
var_dump($isSystemId);

var_dump(IntlTimeZone::getCanonicalID('GMT +01:25', $isSystemId));
var_dump($isSystemId);

?>
==DONE==
--EXPECT--
string(13) "Europe/Lisbon"
bool(true)
string(0) ""
bool(false)
==DONE==