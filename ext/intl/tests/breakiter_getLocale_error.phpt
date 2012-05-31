--TEST--
BreakIterator::getLocale(): arg errors
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$bi = new RuleBasedBreakiterator('[\p{Letter}\uFFFD]+;[:number:]+;');
$bi->setText("\x80sdfé\x90d888 dfsa9");

var_dump($bi->getLocale(1, 2));
var_dump($bi->getLocale(array()));
var_dump($bi->getLocale());

--EXPECTF--

Warning: BreakIterator::getLocale() expects exactly 1 parameter, 2 given in %s on line %d

Warning: BreakIterator::getLocale(): breakiter_get_locale: bad arguments in %s on line %d
bool(false)

Warning: BreakIterator::getLocale() expects parameter 1 to be long, array given in %s on line %d

Warning: BreakIterator::getLocale(): breakiter_get_locale: bad arguments in %s on line %d
bool(false)

Warning: BreakIterator::getLocale() expects exactly 1 parameter, 0 given in %s on line %d

Warning: BreakIterator::getLocale(): breakiter_get_locale: bad arguments in %s on line %d
bool(false)
