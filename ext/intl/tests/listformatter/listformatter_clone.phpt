--TEST--
Test IntlListFormatter cannot be cloned
--SKIPIF--
<?php
if (!extension_loaded('intl')) {
    die('skip intl extension not available');
}
?>
--FILE--
<?php

$formatter = new IntlListFormatter('en_US', IntlListFormatter::TYPE_AND, IntlListFormatter::WIDTH_WIDE);

try {
    $clonedFormatter = clone $formatter;
} catch(Error $error) {
    echo $error::class, ': ', $error->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Error: Trying to clone an uncloneable object of class IntlListFormatter
