--TEST--
Test Uri\Rfc3986\Uri parsing - basic - URI contains null byte
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("https://exam\0ple.com");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Uri\Rfc3986\Uri::__construct(): Argument #1 ($uri) must not contain any null bytes
