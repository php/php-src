--TEST--
Test IntlNumberRangeFormatter::createFromSkeleton throws error for ICU < 63
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (version_compare(INTL_ICU_VERSION, '63.0') >= 0) {
    die('skip for ICU > 63.0');
}
?>
--FILE--
<?php

try {
    $formatter = IntlNumberRangeFormatter::createFromSkeleton(
        'measure-unit/length-meter',
        'en_US',
        IntlNumberRangeFormatter::COLLAPSE_AUTO,
        IntlNumberRangeFormatter::IDENTITY_FALLBACK_SINGLE_VALUE
    );
} catch (Error $error) {
    echo $error->getMessage();
}
?>
--EXPECT--
Class "IntlNumberRangeFormatter" not found