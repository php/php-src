--TEST--
IntlBreakIterator::preceding(): basic test, ICU <= 57.1
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
if (version_compare(INTL_ICU_VERSION, '57.1') > 0) die('skip for ICU <= 57.1');
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
--EXPECT--
int(4)
int(21)
int(0)
