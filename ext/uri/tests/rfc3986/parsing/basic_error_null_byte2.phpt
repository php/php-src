--TEST--
Test Uri\Rfc3986\Uri parsing - basic - URI contains null byte
--FILE--
<?php

try {
    Uri\Rfc3986\Uri::parse("https://exam\0ple.com");
} catch (ValueError $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: Uri\Rfc3986\Uri::parse(): Argument #1 ($uri) must not contain any null bytes
