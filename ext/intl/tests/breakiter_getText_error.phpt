--TEST--
BreakIterator::getText(): arg errors
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$bi = new RuleBasedBreakiterator('[\p{Letter}]+;');
var_dump($bi->getText(array()));

--EXPECTF--

Warning: BreakIterator::getText() expects exactly 0 parameters, 1 given in %s on line %d

Warning: BreakIterator::getText(): breakiter_get_text: bad arguments in %s on line %d
bool(false)
