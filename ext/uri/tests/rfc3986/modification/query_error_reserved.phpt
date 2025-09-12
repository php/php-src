--TEST--
Test Uri\Rfc3986\Uri component modification - query - reserved characters
--EXTENSIONS--
uri
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://example.com");

try {
    $uri->withQuery("#foo");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified query is malformed
