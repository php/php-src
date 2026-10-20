--TEST--
Test Uri\Rfc3986\Uri parsing - scheme - empty
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("://example.com");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidUriException: The specified URI is malformed
