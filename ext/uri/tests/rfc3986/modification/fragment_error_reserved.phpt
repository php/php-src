--TEST--
Test Uri\Rfc3986\Uri::withFragment() - error - reserved characters
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://example.com");

try {
    $uri->withFragment("#fragment");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidUriException: The specified fragment is malformed
