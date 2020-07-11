--TEST--
IntlBreakIterator::getLocale(): basic test
--SKIPIF--
<?php if (!extension_loaded('intl')) die('skip intl extension not enabled'); ?>
<?php if (version_compare(INTL_ICU_VERSION, '64.0') < 0) die('skip for ICU >= 64.0'); ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");

$bi = IntlBreakIterator::createSentenceInstance('pt');

var_dump($bi->getLocale(0));
var_dump($bi->getLocale(1));
?>
--EXPECT--
string(0) ""
string(2) "pt"
