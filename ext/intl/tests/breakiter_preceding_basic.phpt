--TEST--
IntlBreakIterator::preceding(): basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
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
==DONE==
--EXPECT--
int(4)
int(21)
int(0)
==DONE==