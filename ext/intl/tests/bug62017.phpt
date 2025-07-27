--TEST--
Bug #62017: datefmt_create with incorrectly encoded timezone leaks pattern
--EXTENSIONS--
intl
--FILE--
<?php
try {
    datefmt_create('', IntlDateFormatter::NONE, IntlDateFormatter::NONE, "\xFF",
        IntlDateFormatter::GREGORIAN, 'a');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    new IntlDateFormatter('', IntlDateFormatter::NONE, IntlDateFormatter::NONE, "Europe/Lisbon",
            IntlDateFormatter::GREGORIAN, "\x80");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
IntlException: datefmt_create(): Time zone identifier given is not a valid UTF-8 string
IntlException: IntlDateFormatter::__construct(): error converting pattern to UTF-16
