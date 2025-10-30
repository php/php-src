--TEST--
Test Uri\WhatWg\Url parsing - basic - URL contains null byte
--FILE--
<?php

try {
    Uri\WhatWg\Url::parse("https://exam\0ple.com");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Uri\WhatWg\Url::parse(): Argument #1 ($uri) must not contain any null bytes
