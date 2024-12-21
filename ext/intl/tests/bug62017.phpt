--TEST--
Bug #62017: datefmt_create with incorrectly encoded timezone leaks pattern
--EXTENSIONS--
intl
--FILE--
<?php
ini_set('intl.error_level', E_WARNING);
try {
    datefmt_create('', IntlDateFormatter::NONE, IntlDateFormatter::NONE, "\xFF",
        IntlDateFormatter::GREGORIAN, 'a');
} catch (IntlException $e) {
    echo PHP_EOL."Exception: " . $e->getMessage() . " in " . $e->getFile() . " on line " . $e->getLine() . PHP_EOL;
}

try {
        new IntlDateFormatter('', IntlDateFormatter::NONE, IntlDateFormatter::NONE, "Europe/Lisbon",
            IntlDateFormatter::GREGORIAN, "\x80");
} catch (IntlException $e) {
    echo PHP_EOL."Exception: " . $e->getMessage() . " in " . $e->getFile() . " on line " . $e->getLine() . PHP_EOL;
}
?>
--EXPECTF--

Exception: datefmt_create: Time zone identifier given is not a valid UTF-8 string in %s on line %d

Exception: IntlDateFormatter::__construct(): datefmt_create: error converting pattern to UTF-16 in %s on line %d
