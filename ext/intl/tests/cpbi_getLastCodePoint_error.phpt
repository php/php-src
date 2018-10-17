--TEST--
IntlBreakIterator::getLastCodePoint(): bad args
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$it = IntlBreakIterator::createCodePointInstance();
var_dump($it->getLastCodePoint(array()));
--EXPECTF--
Warning: IntlCodePointBreakIterator::getLastCodePoint() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlCodePointBreakIterator::getLastCodePoint(): cpbi_get_last_code_point: bad arguments in %s on line %d
bool(false)
