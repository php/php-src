--TEST--
RuleBasedBreakIterator::__construct(): arg errors
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

//missing ; at the end:
var_dump(new RuleBasedBreakiterator('[\p{Letter}\uFFFD]+;[:number:]+'));
var_dump(new RuleBasedBreakiterator());
var_dump(new RuleBasedBreakiterator(1,2,3));
var_dump(new RuleBasedBreakiterator('[\p{Letter}\uFFFD]+;[:number:]+;', array()));
var_dump(new RuleBasedBreakiterator('[\p{Letter}\uFFFD]+;[:number:]+;', true));

--EXPECTF--

Warning: RuleBasedBreakIterator::__construct(): rbbi_create_instance: unable to create RuleBasedBreakIterator from rules (parse error on line 1, offset 31) in %s on line %d
NULL

Warning: RuleBasedBreakIterator::__construct() expects at least 1 parameter, 0 given in %s on line %d

Warning: RuleBasedBreakIterator::__construct(): rbbi_create_instance: bad arguments in %s on line %d
NULL

Warning: RuleBasedBreakIterator::__construct() expects at most 2 parameters, 3 given in %s on line %d

Warning: RuleBasedBreakIterator::__construct(): rbbi_create_instance: bad arguments in %s on line %d
NULL

Warning: RuleBasedBreakIterator::__construct() expects parameter 2 to be boolean, array given in %s on line %d

Warning: RuleBasedBreakIterator::__construct(): rbbi_create_instance: bad arguments in %s on line %d
NULL

Warning: RuleBasedBreakIterator::__construct(): rbbi_create_instance: unable to creaete instance from compiled rules in %s on line %d
NULL
