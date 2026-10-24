--TEST--
Test Uri\Rfc3986\Uri::withHost() - error - unsetting with port
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri('https://example.com:8080');

try {
    $uri->withHost(null);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidUriException: Cannot remove the host from a URI that has a port
