--TEST--
Test Uri\Rfc3986\Uri component modification - userinfo - reserved characters
--EXTENSIONS--
uri
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://example.com");

try {
    $uri->withUserInfo("us/r:password"); // us/r:password
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified userinfo is malformed
