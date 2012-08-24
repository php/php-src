--TEST--
IntlBreakIterator::getText(): arg errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$bi = new IntlRuleBasedBreakIterator('[\p{Letter}]+;');
var_dump($bi->getText(array()));

--EXPECTF--

Warning: IntlBreakIterator::getText() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlBreakIterator::getText(): breakiter_get_text: bad arguments in %s on line %d
bool(false)
