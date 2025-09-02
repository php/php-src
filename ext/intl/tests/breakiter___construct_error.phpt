--TEST--
IntlRuleBasedBreakIterator::__construct(): arg errors
--EXTENSIONS--
intl
--FILE--
<?php

//missing ; at the end:
try {
    var_dump(new IntlRuleBasedBreakIterator('[\p{Letter}\uFFFD]+;[:number:]+'));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(new IntlRuleBasedBreakIterator());
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(new IntlRuleBasedBreakIterator(1,2,3));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(new IntlRuleBasedBreakIterator('[\p{Letter}\uFFFD]+;[:number:]+;', array()));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(new IntlRuleBasedBreakIterator('[\p{Letter}\uFFFD]+;[:number:]+;', true));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$rbbi = new IntlRuleBasedBreakIterator(".;");
try {
    $rbbi->__construct(".;");
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
IntlException: IntlRuleBasedBreakIterator::__construct(): unable to create RuleBasedBreakIterator from rules (parse error on line 1, offset 31)
ArgumentCountError: IntlRuleBasedBreakIterator::__construct() expects at least 1 argument, 0 given
ArgumentCountError: IntlRuleBasedBreakIterator::__construct() expects at most 2 arguments, 3 given
TypeError: IntlRuleBasedBreakIterator::__construct(): Argument #2 ($compiled) must be of type bool, array given
IntlException: IntlRuleBasedBreakIterator::__construct(): unable to create instance from compiled rules
Error: IntlRuleBasedBreakIterator object is already constructed
