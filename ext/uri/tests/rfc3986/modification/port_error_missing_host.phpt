--TEST--
Test Uri\Rfc3986\Uri component modification - port - error - missing host
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("/foo");

try {
    $uri->withPort(1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidUriException: Cannot set a port without having a host
