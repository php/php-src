--TEST--
IntlListFormatter: error messages
--EXTENSIONS--
intl
--FILE--
<?php

try {
    $formatter = new IntlListFormatter('f', IntlListFormatter::TYPE_AND, IntlListFormatter::WIDTH_WIDE);
} catch(ValueError $exception) {
    echo $exception->getMessage() . PHP_EOL;
}

try {
    $formatter = new IntlListFormatter('ro_thisiswaytooooooooooooooooooooooooooooooooooooooooooooolongtobevaliditneedstobeatleast157characterstofailthevalidationinthelistformattercodeimplementation', IntlListFormatter::TYPE_AND, IntlListFormatter::WIDTH_WIDE);
} catch(ValueError $exception) {
    echo $exception->getMessage() .  PHP_EOL;
}

$formatter = new IntlListFormatter('ro', IntlListFormatter::TYPE_AND, IntlListFormatter::WIDTH_WIDE);

try {
    echo $formatter->format([new stdClass()]);
} catch(Error $error) {
    echo $error->getMessage() . PHP_EOL;
}

try {
    echo $formatter->format([1, 2, new stdClass(), 4]);
} catch(Error $error) {
    echo $error->getMessage()  . PHP_EOL;
}
?>
--EXPECT--
IntlListFormatter::__construct(): Argument #1 ($locale) "f" is invalid
IntlListFormatter::__construct(): Argument #1 ($locale) Locale string too long, should be no longer than 156 characters
Object of class stdClass could not be converted to string
Object of class stdClass could not be converted to string
