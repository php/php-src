--TEST--
BreakIterator::following()/preceding()/isBoundary(): arg errors
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$bi = new RuleBasedBreakiterator('[\p{Letter}\uFFFD]+;[:number:]+;');
$bi->setText("\x80sdfé\x90d888 dfsa9");

var_dump($bi->following(1, 2));
var_dump($bi->following(array()));
var_dump($bi->preceding(1, 2));
var_dump($bi->preceding(array()));
var_dump($bi->isBoundary(1, 2));
var_dump($bi->isBoundary(array()));

--EXPECTF--

Warning: BreakIterator::following() expects exactly 1 parameter, 2 given in %s on line %d

Warning: BreakIterator::following(): breakiter_following: bad arguments in %s on line %d
bool(false)

Warning: BreakIterator::following() expects parameter 1 to be long, array given in %s on line %d

Warning: BreakIterator::following(): breakiter_following: bad arguments in %s on line %d
bool(false)

Warning: BreakIterator::preceding() expects exactly 1 parameter, 2 given in %s on line %d

Warning: BreakIterator::preceding(): breakiter_preceding: bad arguments in %s on line %d
bool(false)

Warning: BreakIterator::preceding() expects parameter 1 to be long, array given in %s on line %d

Warning: BreakIterator::preceding(): breakiter_preceding: bad arguments in %s on line %d
bool(false)

Warning: BreakIterator::isBoundary() expects exactly 1 parameter, 2 given in %s on line %d

Warning: BreakIterator::isBoundary(): breakiter_is_boundary: bad arguments in %s on line %d
bool(false)

Warning: BreakIterator::isBoundary() expects parameter 1 to be long, array given in %s on line %d

Warning: BreakIterator::isBoundary(): breakiter_is_boundary: bad arguments in %s on line %d
bool(false)
