--TEST--
BreakIterator::first()/last()/previous()/current(): arg errors
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$bi = new RuleBasedBreakiterator('[\p{Letter}\uFFFD]+;[:number:]+;');
$bi->setText("\x80sdfé\x90d888 dfsa9");

var_dump($bi->first(1));
var_dump($bi->last(1));
var_dump($bi->previous(1));
var_dump($bi->current(1));

--EXPECTF--

Warning: BreakIterator::first() expects exactly 0 parameters, 1 given in %s on line %d

Warning: BreakIterator::first(): breakiter_first: bad arguments in %s on line %d
bool(false)

Warning: BreakIterator::last() expects exactly 0 parameters, 1 given in %s on line %d

Warning: BreakIterator::last(): breakiter_last: bad arguments in %s on line %d
bool(false)

Warning: BreakIterator::previous() expects exactly 0 parameters, 1 given in %s on line %d

Warning: BreakIterator::previous(): breakiter_previous: bad arguments in %s on line %d
bool(false)

Warning: BreakIterator::current() expects exactly 0 parameters, 1 given in %s on line %d

Warning: BreakIterator::current(): breakiter_current: bad arguments in %s on line %d
bool(false)
