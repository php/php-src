--TEST--
IntlRuleBasedBreakIterator::__construct(): arg errors
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

function print_exception($e) {
	echo "\nException: " . $e->getMessage() . " in " . $e->getFile() . " on line " . $e->getLine() . "\n";
}

//missing ; at the end:
try {
	var_dump(new IntlRuleBasedBreakIterator('[\p{Letter}\uFFFD]+;[:number:]+'));
} catch (IntlException $e) {
	print_exception($e);
}
try {
	var_dump(new IntlRuleBasedBreakIterator());
} catch (TypeError $e) {
	print_exception($e);
}
try {
	var_dump(new IntlRuleBasedBreakIterator(1,2,3));
} catch (TypeError $e) {
	print_exception($e);
}
try {
	var_dump(new IntlRuleBasedBreakIterator('[\p{Letter}\uFFFD]+;[:number:]+;', array()));
} catch (TypeError $e) {
	print_exception($e);
}
try {
	var_dump(new IntlRuleBasedBreakIterator('[\p{Letter}\uFFFD]+;[:number:]+;', true));
} catch (IntlException $e) {
	print_exception($e);
}
--EXPECTF--
Exception: IntlRuleBasedBreakIterator::__construct(): rbbi_create_instance: unable to create RuleBasedBreakIterator from rules (parse error on line 1, offset 31) in %s on line %d

Exception: IntlRuleBasedBreakIterator::__construct() expects at least 1 parameter, 0 given in %s on line %d

Exception: IntlRuleBasedBreakIterator::__construct() expects at most 2 parameters, 3 given in %s on line %d

Exception: IntlRuleBasedBreakIterator::__construct() expects parameter 2 to be bool, array given in %s on line %d

Exception: IntlRuleBasedBreakIterator::__construct(): rbbi_create_instance: unable to create instance from compiled rules in %s on line %d
