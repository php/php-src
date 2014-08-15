--TEST--
IntlBreakIterator::getLocale(): arg errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$bi = new IntlRuleBasedBreakIterator('[\p{Letter}\uFFFD]+;[:number:]+;');
$bi->setText("\x80sdfé\x90d888 dfsa9");

var_dump($bi->getLocale(1, 2));
var_dump($bi->getLocale(array()));
var_dump($bi->getLocale());

--EXPECTF--

Warning: IntlBreakIterator::getLocale() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlBreakIterator::getLocale(): breakiter_get_locale: bad arguments in %s on line %d
bool(false)

Warning: IntlBreakIterator::getLocale() expects parameter 1 to be integer, array given in %s on line %d

Warning: IntlBreakIterator::getLocale(): breakiter_get_locale: bad arguments in %s on line %d
bool(false)

Warning: IntlBreakIterator::getLocale() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlBreakIterator::getLocale(): breakiter_get_locale: bad arguments in %s on line %d
bool(false)
