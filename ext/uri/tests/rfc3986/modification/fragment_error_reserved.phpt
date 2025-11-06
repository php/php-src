--TEST--
Test Uri\Rfc3986\Uri component modification - fragment - reserved characters
--EXTENSIONS--
uri
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://example.com");

try {
    $uri->withFragment("#fragment");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The specified fragment is malformed
