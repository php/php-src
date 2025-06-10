--TEST--
Test URI parsing containing null bytes
--EXTENSIONS--
uri
--FILE--
<?php

try {
    new Uri\WhatWg\Url("https://exam\0ple.com");
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

$url = new Uri\WhatWg\Url("https://example.com");
try {
    $url->withHost("exam\0ple.com");
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Uri\WhatWg\Url::__construct(): Argument #1 ($uri) must not contain any null bytes
Uri\WhatWg\Url::withHost(): Argument #1 ($host) must not contain any null bytes
