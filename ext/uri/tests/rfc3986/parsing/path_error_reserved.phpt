--TEST--
Test Uri\Rfc3986\Uri parsing - path - reserved character
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("https://example.com/fo[o/ba]r/");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The specified URI is malformed
