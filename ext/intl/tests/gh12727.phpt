--TEST--
numfmt creation failures
--EXTENSIONS--
intl
--FILE--
<?php

try {
    new NumberFormatter('xx', NumberFormatter::DECIMAL);
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
ValueError: NumberFormatter::__construct(): Argument #1 ($locale) "%s" is invalid
