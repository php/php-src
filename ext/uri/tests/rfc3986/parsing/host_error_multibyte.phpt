--TEST--
Test Uri\Rfc3986\Uri parsing - host - multibyte character
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("https://exḁmple.com");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The specified URI is malformed
