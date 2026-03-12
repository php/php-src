--TEST--
Test Uri\Rfc3986\Uri component modification - path - unicode characters
--EXTENSIONS--
uri
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://example.com");

try {
    $uri->withPath("/ő");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The specified path is malformed
