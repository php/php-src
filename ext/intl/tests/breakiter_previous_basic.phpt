--TEST--
IntlBreakIterator::previous(): basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");

$bi = IntlBreakIterator::createWordInstance('pt');
$bi->setText('foo bar trans');

var_dump($bi->last());
var_dump($bi->previous());
?>
==DONE==
--EXPECT--
int(13)
int(8)
==DONE==
