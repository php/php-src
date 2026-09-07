--TEST--
Test IntlListFormatter cannot be cloned
--EXTENSIONS--
intl
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
