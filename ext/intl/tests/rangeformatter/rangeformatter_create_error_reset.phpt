--TEST--
IntlNumberRangeFormatter resets stale errors
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
try {
    IntlNumberRangeFormatter::createFromSkeleton(
        'invalid skeleton here',
        'en_US',
        IntlNumberRangeFormatter::COLLAPSE_AUTO,
        IntlNumberRangeFormatter::IDENTITY_FALLBACK_RANGE
    );
} catch (IntlException $exception) {
    var_dump(str_contains(intl_get_error_message(), 'U_NUMBER_SKELETON_SYNTAX_ERROR'));
}

$formatter = IntlNumberRangeFormatter::createFromSkeleton(
    '',
    'en_US',
    IntlNumberRangeFormatter::COLLAPSE_AUTO,
    IntlNumberRangeFormatter::IDENTITY_FALLBACK_RANGE
);

var_dump(intl_get_error_code());
var_dump(intl_get_error_message());

try {
    IntlNumberRangeFormatter::createFromSkeleton(
        'invalid skeleton here',
        'en_US',
        IntlNumberRangeFormatter::COLLAPSE_AUTO,
        IntlNumberRangeFormatter::IDENTITY_FALLBACK_RANGE
    );
} catch (IntlException $exception) {
}

$formatter->format(1, 2);

var_dump(intl_get_error_code());
var_dump(intl_get_error_message());
?>
--EXPECT--
bool(true)
int(0)
string(12) "U_ZERO_ERROR"
int(0)
string(12) "U_ZERO_ERROR"
