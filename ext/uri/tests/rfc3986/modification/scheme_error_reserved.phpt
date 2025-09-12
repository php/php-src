--TEST--
Test Uri\Rfc3986\Uri component modification - scheme - URL encoded characters
--EXTENSIONS--
uri
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://example.com");

try {
    $uri->withScheme("https:");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified scheme is malformed
