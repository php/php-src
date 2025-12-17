--TEST--
errors for IntlNumberRangeFormatter
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
    $nrf = IntlNumberRangeFormatter::createFromSkeleton(
        'invalid skeleton here', 
        'ro', 
        IntlNumberRangeFormatter::COLLAPSE_AUTO, 
        IntlNumberRangeFormatter::IDENTITY_FALLBACK_SINGLE_VALUE
    );
} catch (IntlException $exception) {
    echo $exception->getMessage() . PHP_EOL;
}

echo intl_get_error_code() . PHP_EOL;
echo intl_get_error_message() . PHP_EOL;

try {
    new IntlNumberRangeFormatter();
} catch(Error $error) {
    echo $error->getMessage() .  PHP_EOL;
}

try {
    $nrf = IntlNumberRangeFormatter::createFromSkeleton(
        'invalid skeleton here', 
        'ro', 
        34, 
        IntlNumberRangeFormatter::IDENTITY_FALLBACK_SINGLE_VALUE
    );
} catch (ValueError $exception) {
    echo $exception->getMessage() . PHP_EOL;
}

try {
    $nrf = IntlNumberRangeFormatter::createFromSkeleton(
        'invalid skeleton here', 
        'ro', 
        IntlNumberRangeFormatter::COLLAPSE_AUTO, 
        343
    );
} catch (ValueError $exception) {
    echo $exception->getMessage() . PHP_EOL;
}

try {
    $nrf = IntlNumberRangeFormatter::createFromSkeleton(
        'invalid skeleton here', 
        'invalid-language', 
        IntlNumberRangeFormatter::COLLAPSE_AUTO, 
        IntlNumberRangeFormatter::IDENTITY_FALLBACK_SINGLE_VALUE
    );
} catch (ValueError $exception) {
    echo $exception->getMessage() . PHP_EOL;
}

try {
    $nrf = IntlNumberRangeFormatter::createFromSkeleton(
        'invalid skeleton here', 
        'ro_thisiswaytooooooooooooooooooooooooooooooooooooooooooooolongtobevaliditneedstobeatleast157characterstofailthevalidationinthelistformattercodeimplementation', 
        IntlNumberRangeFormatter::COLLAPSE_AUTO, 
        IntlNumberRangeFormatter::IDENTITY_FALLBACK_SINGLE_VALUE
    );
} catch (ValueError $exception) {
    echo $exception->getMessage() . PHP_EOL;
}

?>
--EXPECT--
IntlNumberRangeFormatter::createFromSkeleton(): Failed to create the number skeleton
65811
IntlNumberRangeFormatter::createFromSkeleton(): Failed to create the number skeleton: U_NUMBER_SKELETON_SYNTAX_ERROR
Call to private IntlNumberRangeFormatter::__construct() from global scope
IntlNumberRangeFormatter::createFromSkeleton(): Argument #3 ($collapse) must be one of IntlNumberRangeFormatter::COLLAPSE_AUTO, IntlNumberRangeFormatter::COLLAPSE_NONE, IntlNumberRangeFormatter::COLLAPSE_UNIT, or IntlNumberRangeFormatter::COLLAPSE_ALL
IntlNumberRangeFormatter::createFromSkeleton(): Argument #4 ($identityFallback) must be one of IntlNumberRangeFormatter::IDENTITY_FALLBACK_SINGLE_VALUE, IntlNumberRangeFormatter::IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE, IntlNumberRangeFormatter::IDENTITY_FALLBACK_APPROXIMATELY, or IntlNumberRangeFormatter::IDENTITY_FALLBACK_RANGE
IntlNumberRangeFormatter::createFromSkeleton(): Argument #2 ($locale) "invalid-language" is invalid
IntlNumberRangeFormatter::createFromSkeleton(): Argument #2 ($locale) must be no longer than 156 characters
