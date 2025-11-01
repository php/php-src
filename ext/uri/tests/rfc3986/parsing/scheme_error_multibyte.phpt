--TEST--
Test Uri\Rfc3986\Uri parsing - scheme - multibyte characters
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("ƕŢŢƤƨ://example.com");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The specified URI is malformed
