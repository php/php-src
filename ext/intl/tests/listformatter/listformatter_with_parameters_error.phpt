--TEST--
IntlListFormatter: Test invalid parameters for TYPE and WIDTH
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '67.0') <= 0) die('skip for ICU < 67.0'); ?>
--FILE--
<?php

try {
    $formatter = new IntlListFormatter('ro', 23232323);
} catch(ValueError $exception) {
    echo $exception->getMessage();
}

echo PHP_EOL;

try {
    $formatter = new IntlListFormatter('ro', IntlListFormatter::TYPE_AND, 2323232);
} catch(ValueError $exception) {
    echo $exception->getMessage();
}
?>
--EXPECT--
IntlListFormatter::__construct(): Argument #2 ($type) must be one of IntlListFormatter::TYPE_AND, IntlListFormatter::TYPE_OR, or IntlListFormatter::TYPE_UNITS
IntlListFormatter::__construct(): Argument #3 ($width) must be one of IntlListFormatter::WIDTH_WIDE, IntlListFormatter::WIDTH_SHORT, or IntlListFormatter::WIDTH_NARROW
