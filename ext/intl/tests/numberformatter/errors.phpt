--TEST--
Errors for IntlNumberFormatter
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (!class_exists('IntlNumberFormatter')) {
    die('skip IntlNumberFormatter not available');
}
?>
--FILE--
<?php

var_dump(IntlNumberFormatter::createFromSkeleton('invalid skeleton here', 'ro'));
var_dump(intl_get_error_code());
var_dump(intl_get_error_message());

var_dump(IntlNumberFormatter::createFromSkeleton('measure-unit/length-nope', 'en_US'));
var_dump(intl_get_error_message());

var_dump(IntlNumberFormatter::createFromSkeleton('', 'ro_' . str_repeat('a', 160)));
var_dump(intl_get_error_message());

var_dump(IntlNumberFormatter::createFromSkeleton("percent\xFF", 'en_US'));
var_dump(intl_get_error_message());

$formatter = IntlNumberFormatter::createFromSkeleton('', 'en_US');

var_dump(intl_get_error_code());

ini_set('intl.use_exceptions', '1');

try {
    IntlNumberFormatter::createFromSkeleton('invalid skeleton here', 'ro');
} catch (IntlException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

ini_set('intl.use_exceptions', '0');

try {
    new IntlNumberFormatter();
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    IntlNumberFormatter::createFromSkeleton('', 'invalid-language');
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

IntlNumberFormatter::createFromSkeleton('invalid skeleton here', 'ro');

try {
    $formatter->format([]);
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

var_dump(intl_get_error_code());

try {
    clone $formatter;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
NULL
int(65811)
string(107) "IntlNumberFormatter::createFromSkeleton(): Number formatter creation failed: U_NUMBER_SKELETON_SYNTAX_ERROR"
NULL
string(107) "IntlNumberFormatter::createFromSkeleton(): Number formatter creation failed: U_NUMBER_SKELETON_SYNTAX_ERROR"
NULL
string(132) "IntlNumberFormatter::createFromSkeleton(): Locale string too long, should be no longer than 156 characters: U_ILLEGAL_ARGUMENT_ERROR"
NULL
string(100) "IntlNumberFormatter::createFromSkeleton(): Error converting skeleton to UTF-16: U_INVALID_CHAR_FOUND"
int(0)
IntlException: IntlNumberFormatter::createFromSkeleton(): Number formatter creation failed
Error: Call to private IntlNumberFormatter::__construct() from global scope
ValueError: IntlNumberFormatter::createFromSkeleton(): Argument #2 ($locale) "invalid-language" is invalid
TypeError: IntlNumberFormatter::format(): Argument #1 ($value) must be of type string|int|float, array given
int(65811)
Error: Trying to clone an uncloneable object of class IntlNumberFormatter
