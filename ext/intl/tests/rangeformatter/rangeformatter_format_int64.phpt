--TEST--
IntlNumberRangeFormatter::format() preserves int64 precision
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (!class_exists('IntlNumberRangeFormatter')) {
    die('skip IntlNumberRangeFormatter not available');
}
if (PHP_INT_SIZE < 8) {
    die('skip 64-bit only');
}
?>
--FILE--
<?php
$formatter = IntlNumberRangeFormatter::createFromSkeleton(
    '',
    'en_US',
    IntlNumberRangeFormatter::COLLAPSE_AUTO,
    IntlNumberRangeFormatter::IDENTITY_FALLBACK_SINGLE_VALUE
);

$formatted = $formatter->format(9007199254740993, 9007199254740993);
var_dump(preg_replace('/[^0-9]/', '', $formatted));
?>
--EXPECT--
string(16) "9007199254740993"
