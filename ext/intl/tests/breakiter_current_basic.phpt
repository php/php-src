--TEST--
IntlBreakIterator::current(): basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");

$bi = IntlBreakIterator::createWordInstance('pt');
var_dump($bi->current());
$bi->setText('foo bar trans zoo bee');

var_dump($bi->first());
var_dump($bi->current());
var_dump($bi->next());
var_dump($bi->current());
?>
==DONE==
--EXPECT--
int(0)
int(0)
int(0)
int(3)
int(3)
==DONE==