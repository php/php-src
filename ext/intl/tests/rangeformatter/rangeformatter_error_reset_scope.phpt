--TEST--
IntlNumberRangeFormatter keeps the intl error state on a parameter error
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
    'en_US',
    IntlNumberRangeFormatter::COLLAPSE_AUTO,
    IntlNumberRangeFormatter::IDENTITY_FALLBACK_RANGE
);

try {
    IntlNumberRangeFormatter::createFromSkeleton(
        'invalid skeleton here',
        'en_US',
        IntlNumberRangeFormatter::COLLAPSE_AUTO,
        IntlNumberRangeFormatter::IDENTITY_FALLBACK_RANGE
    );
} catch (IntlException $exception) {
}

try {
    $formatter->format([], 2);
} catch (TypeError $error) {
    echo $error::class, ': ', $error->getMessage(), PHP_EOL;
}

var_dump(intl_get_error_code() !== 0);

try {
    IntlNumberRangeFormatter::createFromSkeleton(
        [],
        'en_US',
        IntlNumberRangeFormatter::COLLAPSE_AUTO,
        IntlNumberRangeFormatter::IDENTITY_FALLBACK_RANGE
    );
} catch (TypeError $error) {
    echo $error::class, ': ', $error->getMessage(), PHP_EOL;
}

var_dump(intl_get_error_code() !== 0);

$formatter->format(1, 2);

var_dump(intl_get_error_code());
var_dump($formatter->getErrorCode());

?>
--EXPECT--
TypeError: IntlNumberRangeFormatter::format(): Argument #1 ($start) must be of type int|float, array given
bool(true)
TypeError: IntlNumberRangeFormatter::createFromSkeleton(): Argument #1 ($skeleton) must be of type string, array given
bool(true)
int(0)
int(0)
