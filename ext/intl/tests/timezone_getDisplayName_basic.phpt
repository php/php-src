--TEST--
IntlTimeZone::getDisplayName(): basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$lsb = IntlTimeZone::createTimeZone('Europe/Lisbon');

ini_set('intl.default_locale', 'en_US');
var_dump($lsb->getDisplayName());

ini_set('intl.default_locale', 'pt_PT');
var_dump($lsb->getDisplayName());

?>
--EXPECTF--
string(%d) "Western European%sTime"
string(%d) "Hora%sda Europa Ocidental"