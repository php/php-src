--TEST--
IntlNumberFormatter::format() with a numeric string
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

$formatter = IntlNumberFormatter::createFromSkeleton('precision-unlimited', 'en_US');

var_dump($formatter->format('12345678901234567890.123456789'));
var_dump($formatter->format('9223372036854775808'));
var_dump($formatter->format('-0.1'));

var_dump(IntlNumberFormatter::createFromSkeleton('', 'en_US')->format('12345678901234567890.123456789'));
var_dump(IntlNumberFormatter::createFromSkeleton('measure-unit/length-meter .00', 'de')->format('1234.5'));

foreach (['abc', '', ' 12', '1,5', '1e3'] as $value) {
    var_dump($formatter->format($value));
    var_dump($formatter->getErrorMessage());
}

var_dump($formatter->format('12'));
var_dump($formatter->getErrorCode());

try {
    $formatter->format("1\0002");
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
string(36) "12,345,678,901,234,567,890.123456789"
string(25) "9,223,372,036,854,775,808"
string(4) "-0.1"
string(33) "12,345,678,901,234,567,890.123457"
string(10) "1.234,50 m"
bool(false)
string(86) "IntlNumberFormatter::format(): Number formatting failed: U_DECIMAL_NUMBER_SYNTAX_ERROR"
bool(false)
string(86) "IntlNumberFormatter::format(): Number formatting failed: U_DECIMAL_NUMBER_SYNTAX_ERROR"
bool(false)
string(86) "IntlNumberFormatter::format(): Number formatting failed: U_DECIMAL_NUMBER_SYNTAX_ERROR"
bool(false)
string(86) "IntlNumberFormatter::format(): Number formatting failed: U_DECIMAL_NUMBER_SYNTAX_ERROR"
bool(false)
string(86) "IntlNumberFormatter::format(): Number formatting failed: U_DECIMAL_NUMBER_SYNTAX_ERROR"
string(2) "12"
int(0)
ValueError: IntlNumberFormatter::format(): Argument #1 ($value) must not contain any null bytes
