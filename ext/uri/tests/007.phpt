--TEST--
Test manual Uri child instance creation error cases
--EXTENSIONS--
uri
--FILE--
<?php

try {
    new Uri\Rfc3986Uri("https://example.com:8080@username:password/path?q=r#fragment");
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    new Uri\WhatWgUri("https://example.com:8080@username:password/path?q=r#fragment");
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
Uri\Rfc3986Uri::__construct(): Argument #1 ($uri) must be a valid URI
Uri\WhatWgUri::__construct(): Argument #1 ($uri) must be a valid URI
