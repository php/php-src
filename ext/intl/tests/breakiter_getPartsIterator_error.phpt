--TEST--
IntlBreakIterator::getPartsIterator(): bad args
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");

$it = IntlBreakIterator::createWordInstance(NULL);
var_dump($it->getPartsIterator(array()));
var_dump($it->getPartsIterator(1, 2));
var_dump($it->getPartsIterator(-1));

?>
==DONE==
--EXPECTF--
Warning: IntlBreakIterator::getPartsIterator() expects parameter 1 to be integer, array given in %s on line %d

Warning: IntlBreakIterator::getPartsIterator(): breakiter_get_parts_iterator: bad arguments in %s on line %d
bool(false)

Warning: IntlBreakIterator::getPartsIterator() expects at most 1 parameter, 2 given in %s on line %d

Warning: IntlBreakIterator::getPartsIterator(): breakiter_get_parts_iterator: bad arguments in %s on line %d
bool(false)

Warning: IntlBreakIterator::getPartsIterator(): breakiter_get_parts_iterator: bad key type in %s on line %d
bool(false)
==DONE==
