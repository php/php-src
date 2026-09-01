--TEST--
Test Uri\Rfc3986\Uri component modification - host - error - unsetting with userinfo
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("ftp://user:pass@foo.com?query=abc#foo");

try {
    $uri->withHost(null);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidUriException: Cannot remove the host from a URI that has a userinfo
