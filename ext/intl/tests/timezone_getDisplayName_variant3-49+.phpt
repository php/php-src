--TEST--
IntlTimeZone::getDisplayName(): locale parameter
--SKIPIF--
<?php if (!extension_loaded('intl')) die('skip intl extension not enabled'); ?>
<?php if (version_compare(INTL_ICU_VERSION, '55.1') >= 0) die('skip for ICU < 55.1'); ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("error_reporting", -1);
ini_set("display_errors", 1);

$lsb = IntlTimeZone::createTimeZone('Europe/Lisbon');

ini_set('intl.default_locale', 'en_US');
var_dump($lsb->getDisplayName(false, IntlTimeZone::DISPLAY_LONG));
var_dump($lsb->getDisplayName(false, IntlTimeZone::DISPLAY_LONG, NULL));
var_dump($lsb->getDisplayName(false, IntlTimeZone::DISPLAY_LONG, 'pt_PT'));

?>
--EXPECTF--
string(30) "Western European Standard Time"
string(30) "Western European Standard Time"
string(32) "Hora %cadrão da Europa Ocidental"
