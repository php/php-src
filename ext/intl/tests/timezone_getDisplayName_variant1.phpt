--TEST--
IntlTimeZone::getDisplayName(): daylight parameter effect
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("error_reporting", -1);
ini_set("display_errors", 1);

$lsb = IntlTimeZone::createTimeZone('Europe/Lisbon');

ini_set('intl.default_locale', 'en_US');
var_dump($lsb->getDisplayName());
var_dump($lsb->getDisplayName(false));
var_dump($lsb->getDisplayName(true));

?>
==DONE==
--EXPECTF--
string(%d) "Western European%sTime"
string(%d) "Western European%sTime"
string(28) "Western European Summer Time"
==DONE==