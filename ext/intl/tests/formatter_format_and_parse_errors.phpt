--TEST--
ValueErrors for format/parse methods and procedural functions
--EXTENSIONS--
intl
--FILE--
<?php

$o = new NumberFormatter('en_US', NumberFormatter::PATTERN_DECIMAL);
$num = 5;
$str = "string";

/* Unknown type constant */
try {
    numfmt_format($o, $num, -20);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    $o->format($num, -20);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    numfmt_parse($o, $str, -20);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    $o->parse($str, -20);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

/* With NumberFormatter::TYPE_CURRENCY */
try {
    numfmt_format($o, $num, NumberFormatter::TYPE_CURRENCY);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    $o->format($num, NumberFormatter::TYPE_CURRENCY);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    numfmt_parse($o, $str, NumberFormatter::TYPE_CURRENCY);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    $o->parse($str, NumberFormatter::TYPE_CURRENCY);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

?>
--EXPECTF--
numfmt_format(): Argument #3 ($type) must be a NumberFormatter::TYPE_* constant
NumberFormatter::format(): Argument #2 ($type) must be a NumberFormatter::TYPE_* constant
numfmt_parse(): Argument #3 ($type) must be a NumberFormatter::TYPE_* constant
NumberFormatter::parse(): Argument #2 ($type) must be a NumberFormatter::TYPE_* constant

Deprecated: Constant NumberFormatter::TYPE_CURRENCY is deprecated in %s on line %d
numfmt_format(): Argument #3 ($type) cannot be NumberFormatter::TYPE_CURRENCY constant, use numfmt_format_currency() function instead

Deprecated: Constant NumberFormatter::TYPE_CURRENCY is deprecated in %s on line %d
NumberFormatter::format(): Argument #2 ($type) cannot be NumberFormatter::TYPE_CURRENCY constant, use NumberFormatter::formatCurrency() method instead

Deprecated: Constant NumberFormatter::TYPE_CURRENCY is deprecated in %s on line %d
numfmt_parse(): Argument #3 ($type) cannot be NumberFormatter::TYPE_CURRENCY constant, use numfmt_parse_currency() function instead

Deprecated: Constant NumberFormatter::TYPE_CURRENCY is deprecated in %s on line %d
NumberFormatter::parse(): Argument #2 ($type) cannot be NumberFormatter::TYPE_CURRENCY constant, use NumberFormatter::parseCurrency() method instead
