--TEST--
IntlBreakIterator::first()/last()/previous()/current(): arg errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$bi = new IntlRuleBasedBreakIterator('[\p{Letter}\uFFFD]+;[:number:]+;');
$bi->setText("\x80sdfé\x90d888 dfsa9");

var_dump($bi->first(1));
var_dump($bi->last(1));
var_dump($bi->previous(1));
var_dump($bi->current(1));
--EXPECTF--
Warning: IntlBreakIterator::first() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlBreakIterator::first(): breakiter_first: bad arguments in %s on line %d
bool(false)

Warning: IntlBreakIterator::last() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlBreakIterator::last(): breakiter_last: bad arguments in %s on line %d
bool(false)

Warning: IntlBreakIterator::previous() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlBreakIterator::previous(): breakiter_previous: bad arguments in %s on line %d
bool(false)

Warning: IntlBreakIterator::current() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlBreakIterator::current(): breakiter_current: bad arguments in %s on line %d
bool(false)
