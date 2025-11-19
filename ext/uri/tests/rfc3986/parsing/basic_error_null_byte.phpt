--TEST--
Test Uri\Rfc3986\Uri parsing - basic - URI contains null byte
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("https://exam\0ple.com");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The specified URI is malformed
