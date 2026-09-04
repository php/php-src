--TEST--
Test Uri\Rfc3986\Uri parsing - port - percent-encoded character
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("https://example.com:%30");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidUriException: The specified URI is malformed
