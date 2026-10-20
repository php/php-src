--TEST--
Test Uri\Rfc3986\Uri::withUserInfo() - error - reserved characters
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://example.com");

try {
    $uri->withUserInfo("us/r:password"); // us/r:password
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidUriException: The specified userinfo is malformed
