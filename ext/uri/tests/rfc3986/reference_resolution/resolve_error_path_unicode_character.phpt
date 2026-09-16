--TEST--
Test Uri\Rfc3986\Uri::resolve() - error - Unicode character in path
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("https://example.com");

try {
    $uri->resolve("á");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidUriException: The specified URI is malformed
