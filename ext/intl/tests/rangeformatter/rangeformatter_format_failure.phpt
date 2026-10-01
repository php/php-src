--TEST--
IntlNumberRangeFormatter::format() with a failing formatter
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (version_compare(INTL_ICU_VERSION, '63.0') < 0) {
    die('skip for ICU < 63.0');
}
?>
--FILE--
<?php

$formatter = IntlNumberRangeFormatter::createFromSkeleton(
    '',
    'en_US@numbers=foobar',
    IntlNumberRangeFormatter::COLLAPSE_AUTO,
    IntlNumberRangeFormatter::IDENTITY_FALLBACK_RANGE
);

try {
    $formatter->format(1, 2);
} catch (IntlException $exception) {
    echo $exception::class, ': ', $exception->getMessage(), PHP_EOL;
}

var_dump($formatter->getErrorCode() !== 0);
var_dump(str_starts_with(
    $formatter->getErrorMessage(),
    'IntlNumberRangeFormatter::format(): Failed to format number range: '
));

var_dump(intl_get_error_code() !== 0);

$formatter = IntlNumberRangeFormatter::createFromSkeleton(
    '',
    'en_US',
    IntlNumberRangeFormatter::COLLAPSE_AUTO,
    IntlNumberRangeFormatter::IDENTITY_FALLBACK_RANGE
);

var_dump($formatter->format(1, 2) !== '');
var_dump($formatter->getErrorCode());
var_dump($formatter->getErrorMessage());

?>
--EXPECT--
IntlException: IntlNumberRangeFormatter::format(): Failed to format number range
bool(true)
bool(true)
bool(true)
bool(true)
int(0)
string(12) "U_ZERO_ERROR"
