--TEST--
Test URI parsing containing null bytes
--EXTENSIONS--
uri
--FILE--
<?php

try {
    new Uri\Rfc3986Uri("https://exam\0ple.com");
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

$uri = new Uri\Rfc3986Uri("https://example.com");
try {
    $uri->withHost("exam\0ple.com");
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    new Uri\WhatWgUri("https://exam\0ple.com");
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

$uri = new Uri\WhatWgUri("https://example.com");
try {
    $uri->withHost("exam\0ple.com");
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Uri\Rfc3986Uri::__construct(): Argument #1 ($uri) must not contain any null bytes
Uri\Rfc3986Uri::withHost(): Argument #1 ($encodedHost) must not contain any null bytes
Uri\WhatWgUri::__construct(): Argument #1 ($uri) must not contain any null bytes
Uri\WhatWgUri::withHost(): Argument #1 ($encodedHost) must not contain any null bytes
