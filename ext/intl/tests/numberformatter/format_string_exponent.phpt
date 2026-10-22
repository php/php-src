--TEST--
IntlNumberFormatter::format() rejects numeric strings in exponent notation
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (!class_exists('IntlNumberFormatter')) {
    die('skip IntlNumberFormatter not available');
}
?>
--INI--
memory_limit=16M
--FILE--
<?php

$formatter = IntlNumberFormatter::createFromSkeleton('precision-unlimited', 'en_US');

var_dump(strlen($formatter->format('1' . str_repeat('0', 999))));

foreach (['1e3', '1E-3', '1e999999999'] as $value) {
    var_dump($formatter->format($value));
    var_dump($formatter->getErrorMessage());
}

ini_set('intl.use_exceptions', '1');

try {
    $formatter->format('1e999999999');
} catch (IntlException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
int(1333)
bool(false)
string(86) "IntlNumberFormatter::format(): Number formatting failed: U_DECIMAL_NUMBER_SYNTAX_ERROR"
bool(false)
string(86) "IntlNumberFormatter::format(): Number formatting failed: U_DECIMAL_NUMBER_SYNTAX_ERROR"
bool(false)
string(86) "IntlNumberFormatter::format(): Number formatting failed: U_DECIMAL_NUMBER_SYNTAX_ERROR"
IntlException: IntlNumberFormatter::format(): Number formatting failed
