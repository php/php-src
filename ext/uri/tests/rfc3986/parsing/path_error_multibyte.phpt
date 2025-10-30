--TEST--
Test Uri\Rfc3986\Uri parsing - path - multibyte character
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("https://example.com/fȎȎ");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The specified URI is malformed
