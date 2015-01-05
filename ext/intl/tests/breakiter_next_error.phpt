--TEST--
IntlBreakIterator::next(): arg errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$bi = new IntlRuleBasedBreakIterator('[\p{Letter}\uFFFD]+;[:number:]+;');
$bi->setText("\x80sdfé\x90d888 dfsa9");

var_dump($bi->next(1, 2));
var_dump($bi->next(array()));

--EXPECTF--

Warning: IntlBreakIterator::next() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlBreakIterator::next(): breakiter_next: bad arguments in %s on line %d
bool(false)

Warning: IntlBreakIterator::next() expects parameter 1 to be integer, array given in %s on line %d

Warning: IntlBreakIterator::next(): breakiter_next: bad arguments in %s on line %d
bool(false)
