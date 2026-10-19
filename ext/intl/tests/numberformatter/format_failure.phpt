--TEST--
IntlNumberFormatter::format() with a failing formatter
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

$formatter = IntlNumberFormatter::createFromSkeleton('', 'en_US@numbers=foobar');

var_dump($formatter->format(1));
var_dump($formatter->getErrorCode() === U_UNSUPPORTED_ERROR);
var_dump($formatter->getErrorMessage());
var_dump(intl_get_error_code() === U_UNSUPPORTED_ERROR);

ini_set('intl.use_exceptions', '1');

try {
    $formatter->format(1);
} catch (IntlException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

ini_set('intl.use_exceptions', '0');

$formatter = IntlNumberFormatter::createFromSkeleton('', 'en_US');

var_dump($formatter->format(1));
var_dump(intl_get_error_code());
var_dump($formatter->getErrorCode());
var_dump($formatter->getErrorMessage());

?>
--EXPECT--
bool(false)
bool(true)
string(76) "IntlNumberFormatter::format(): Number formatting failed: U_UNSUPPORTED_ERROR"
bool(true)
IntlException: IntlNumberFormatter::format(): Number formatting failed
string(1) "1"
int(0)
int(0)
string(12) "U_ZERO_ERROR"
