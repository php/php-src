--TEST--
IntlBreakIterator::preceding(): basic test, ICU >= 58.1
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (version_compare(INTL_ICU_VERSION, '57.1') <= 0) die('skip for ICU >= 58.1');
?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");

$bi = IntlBreakIterator::createWordInstance('pt');
$bi->setText('foo bar trans zoo bee');

var_dump($bi->preceding(5));
var_dump($bi->preceding(50));
var_dump($bi->preceding(-1));
?>
--EXPECTF--
int(4)
int(21)
int(%i)
