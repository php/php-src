--TEST--
BreakIterator::next(): arg errors
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$bi = new RuleBasedBreakiterator('[\p{Letter}\uFFFD]+;[:number:]+;');
$bi->setText("\x80sdfé\x90d888 dfsa9");

var_dump($bi->next(1, 2));
var_dump($bi->next(array()));

--EXPECTF--

Warning: BreakIterator::next() expects exactly 1 parameter, 2 given in %s on line %d

Warning: BreakIterator::next(): breakiter_next: bad arguments in %s on line %d
bool(false)

Warning: BreakIterator::next() expects parameter 1 to be long, array given in %s on line %d

Warning: BreakIterator::next(): breakiter_next: bad arguments in %s on line %d
bool(false)
