--TEST--
IntlBreakIterator::setText(): arg errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$bi = new IntlRuleBasedBreakIterator('[\p{Letter}]+;');
var_dump($bi->setText());
var_dump($bi->setText(array()));
var_dump($bi->setText(1,2));

class A {
function __destruct() { var_dump('destructed'); throw new Exception('e'); }
function __tostring() { return 'foo'; }
}

try {
var_dump($bi->setText(new A));
} catch (Exception $e) {
var_dump($e->getMessage());
}
--EXPECTF--
Warning: IntlBreakIterator::setText() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlBreakIterator::setText(): breakiter_set_text: bad arguments in %s on line %d
bool(false)

Warning: IntlBreakIterator::setText() expects parameter 1 to be string, array given in %s on line %d

Warning: IntlBreakIterator::setText(): breakiter_set_text: bad arguments in %s on line %d
bool(false)

Warning: IntlBreakIterator::setText() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlBreakIterator::setText(): breakiter_set_text: bad arguments in %s on line %d
bool(false)
string(10) "destructed"
string(1) "e"
