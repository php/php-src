--TEST--
IntlRuleBasedBreakIterator::__construct(): arg errors
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if(version_compare(INTL_ICU_VERSION, '4.8') < 0) print 'skip ICU >= 4.8 only'; ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

//missing ; at the end:
var_dump(new IntlRuleBasedBreakIterator('[\p{Letter}\uFFFD]+;[:number:]+'));
var_dump(new IntlRuleBasedBreakIterator());
var_dump(new IntlRuleBasedBreakIterator(1,2,3));
var_dump(new IntlRuleBasedBreakIterator('[\p{Letter}\uFFFD]+;[:number:]+;', array()));
var_dump(new IntlRuleBasedBreakIterator('[\p{Letter}\uFFFD]+;[:number:]+;', true));

--EXPECTF--

Warning: IntlRuleBasedBreakIterator::__construct(): rbbi_create_instance: unable to create RuleBasedBreakIterator from rules (parse error on line 1, offset 31) in %s on line %d
NULL

Warning: IntlRuleBasedBreakIterator::__construct() expects at least 1 parameter, 0 given in %s on line %d

Warning: IntlRuleBasedBreakIterator::__construct(): rbbi_create_instance: bad arguments in %s on line %d
NULL

Warning: IntlRuleBasedBreakIterator::__construct() expects at most 2 parameters, 3 given in %s on line %d

Warning: IntlRuleBasedBreakIterator::__construct(): rbbi_create_instance: bad arguments in %s on line %d
NULL

Warning: IntlRuleBasedBreakIterator::__construct() expects parameter 2 to be boolean, array given in %s on line %d

Warning: IntlRuleBasedBreakIterator::__construct(): rbbi_create_instance: bad arguments in %s on line %d
NULL

Warning: IntlRuleBasedBreakIterator::__construct(): rbbi_create_instance: unable to create instance from compiled rules in %s on line %d
NULL
