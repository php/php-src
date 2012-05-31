--TEST--
BreakIterator::last(): basic test
--SKIPIF--
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$bi = BreakIterator::createWordInstance('pt');
$bi->setText('foo bar trans');

var_dump($bi->current());
var_dump($bi->last());
var_dump($bi->current());
--EXPECTF--
int(0)
int(13)
int(13)
