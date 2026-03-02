--TEST--
GH-8115 (Can't catch deprecation in IntlDateFormatter)
--EXTENSIONS--
intl
--FILE--
<?php

error_reporting(E_ALL);
set_error_handler(function ($errNo, $errStr) {
    echo "Caught ($errNo): $errStr\n";
});
try {
    new \IntlDateFormatter(null, null, null);
} catch (\IntlException) {}
try {
    new \IntlRuleBasedBreakIterator(null, null);
} catch (\IntlException) {}
// Can't be tested since all params are optional
new \IntlGregorianCalendar(null, null);
new \Collator(null);
// Can't be tested since all params are optional
new \IntlDatePatternGenerator(null);
new \NumberFormatter(null, null);
try {
    new \MessageFormatter(null, null);
} catch (\IntlException) {}
new \ResourceBundle(null, null, null);

?>
--EXPECT--
Caught (8192): IntlDateFormatter::__construct(): Passing null to parameter #2 ($dateType) of type int is deprecated
Caught (8192): IntlDateFormatter::__construct(): Passing null to parameter #3 ($timeType) of type int is deprecated
Caught (8192): IntlRuleBasedBreakIterator::__construct(): Passing null to parameter #1 ($rules) of type string is deprecated
Caught (8192): IntlRuleBasedBreakIterator::__construct(): Passing null to parameter #2 ($compiled) of type bool is deprecated
Caught (8192): Collator::__construct(): Passing null to parameter #1 ($locale) of type string is deprecated
Caught (8192): NumberFormatter::__construct(): Passing null to parameter #1 ($locale) of type string is deprecated
Caught (8192): NumberFormatter::__construct(): Passing null to parameter #2 ($style) of type int is deprecated
Caught (8192): MessageFormatter::__construct(): Passing null to parameter #1 ($locale) of type string is deprecated
Caught (8192): MessageFormatter::__construct(): Passing null to parameter #2 ($pattern) of type string is deprecated
Caught (8192): ResourceBundle::__construct(): Passing null to parameter #3 ($fallback) of type bool is deprecated
