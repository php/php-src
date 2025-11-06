--TEST--
Test Uri\Rfc3986\Uri component modification - scheme - reserved characters
--EXTENSIONS--
uri
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://example.com");

try {
    $uri->withScheme("https:");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The specified scheme is malformed
