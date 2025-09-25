--TEST--
Test IntlNumberRangeFormatter cannot be cloned
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
    'measure-unit/length-meter',
    'en_US',
    IntlNumberRangeFormatter::COLLAPSE_AUTO,
    IntlNumberRangeFormatter::IDENTITY_FALLBACK_SINGLE_VALUE
);

try {
    $clonedFormatter = clone $formatter;
} catch(Error $error) {
    echo $error->getMessage();
}
?>
--EXPECT--
Trying to clone an uncloneable object of class IntlNumberRangeFormatter 
