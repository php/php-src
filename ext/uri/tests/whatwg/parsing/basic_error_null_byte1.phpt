--TEST--
Test Uri\WhatWg\Url parsing - basic - URL contains null byte
--FILE--
<?php

try {
    new Uri\WhatWg\Url("https://exam\0ple.com");
} catch (ValueError $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: Uri\WhatWg\Url::__construct(): Argument #1 ($uri) must not contain any null bytes
