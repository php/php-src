--TEST--
NumberFormatter parse offset overflow
--EXTENSIONS--
intl
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip: 64-bit only"); ?>
--FILE--
<?php
$fmt = new NumberFormatter('en_US', NumberFormatter::DECIMAL);
$currencyFmt = new NumberFormatter('en_US', NumberFormatter::CURRENCY);

function print_error(callable $callback): void {
    try {
        $callback();
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

$offset = PHP_INT_MAX;
print_error(function () use ($fmt, &$offset) {
    $fmt->parse('123', NumberFormatter::TYPE_DOUBLE, $offset);
});

$offset = PHP_INT_MAX;
print_error(function () use ($fmt, &$offset) {
    numfmt_parse($fmt, '123', NumberFormatter::TYPE_DOUBLE, $offset);
});

$currency = '';
$offset = PHP_INT_MAX;
print_error(function () use ($currencyFmt, &$currency, &$offset) {
    $currencyFmt->parseCurrency('$123.00', $currency, $offset);
});

$currency = '';
$offset = PHP_INT_MAX;
print_error(function () use ($currencyFmt, &$currency, &$offset) {
    numfmt_parse_currency($currencyFmt, '$123.00', $currency, $offset);
});
?>
--EXPECT--
ValueError: NumberFormatter::parse(): Argument #3 ($offset) must be between -2147483648 and 2147483647
ValueError: numfmt_parse(): Argument #4 ($offset) must be between -2147483648 and 2147483647
ValueError: NumberFormatter::parseCurrency(): Argument #3 ($offset) must be between -2147483648 and 2147483647
ValueError: numfmt_parse_currency(): Argument #4 ($offset) must be between -2147483648 and 2147483647
