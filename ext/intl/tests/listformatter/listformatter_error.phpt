--TEST--
IntlListFormatter: error messages
--EXTENSIONS--
intl
--FILE--
<?php

try {
    $formatter = new IntlListFormatter('f', IntlListFormatter::TYPE_AND, IntlListFormatter::WIDTH_WIDE);
} catch(ValueError $exception) {
    echo $exception->getMessage();
}

echo PHP_EOL;

try {
    $formatter = new IntlListFormatter('ro', IntlListFormatter::TYPE_AND, 23);
} catch (IntlException $exception) {
    echo $exception->getMessage() .PHP_EOL;
    echo intl_get_error_code();
}

$formatter = new IntlListFormatter('ro', IntlListFormatter::TYPE_AND, IntlListFormatter::WIDTH_WIDE);

echo PHP_EOL;
try {
    echo $formatter->format([new stdClass()]);
} catch(Error $error) {
    echo $error->getMessage();
}

--EXPECT--
IntlListFormatter::__construct(): Argument #1 ($locale) "f" is invalid
Constructor failed
1
Object of class stdClass could not be converted to string