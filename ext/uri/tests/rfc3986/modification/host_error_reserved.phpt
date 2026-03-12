--TEST--
Test Uri\Rfc3986\Uri component modification - host - reserved characters
--EXTENSIONS--
uri
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://example.com");

try {
    $uri->withHost("ex#mple.com");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The specified host is malformed
