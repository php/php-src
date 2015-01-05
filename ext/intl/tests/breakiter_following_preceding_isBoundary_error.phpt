--TEST--
IntlBreakIterator::following()/preceding()/isBoundary(): arg errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$bi = new IntlRuleBasedBreakIterator('[\p{Letter}\uFFFD]+;[:number:]+;');
$bi->setText("\x80sdfé\x90d888 dfsa9");

var_dump($bi->following(1, 2));
var_dump($bi->following(array()));
var_dump($bi->preceding(1, 2));
var_dump($bi->preceding(array()));
var_dump($bi->isBoundary(1, 2));
var_dump($bi->isBoundary(array()));

--EXPECTF--

Warning: IntlBreakIterator::following() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlBreakIterator::following(): breakiter_following: bad arguments in %s on line %d
bool(false)

Warning: IntlBreakIterator::following() expects parameter 1 to be integer, array given in %s on line %d

Warning: IntlBreakIterator::following(): breakiter_following: bad arguments in %s on line %d
bool(false)

Warning: IntlBreakIterator::preceding() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlBreakIterator::preceding(): breakiter_preceding: bad arguments in %s on line %d
bool(false)

Warning: IntlBreakIterator::preceding() expects parameter 1 to be integer, array given in %s on line %d

Warning: IntlBreakIterator::preceding(): breakiter_preceding: bad arguments in %s on line %d
bool(false)

Warning: IntlBreakIterator::isBoundary() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlBreakIterator::isBoundary(): breakiter_is_boundary: bad arguments in %s on line %d
bool(false)

Warning: IntlBreakIterator::isBoundary() expects parameter 1 to be integer, array given in %s on line %d

Warning: IntlBreakIterator::isBoundary(): breakiter_is_boundary: bad arguments in %s on line %d
bool(false)
