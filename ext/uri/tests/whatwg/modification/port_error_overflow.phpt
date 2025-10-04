--TEST--
Test Uri\WhatWg\Url component modification - port - too large port numbers don't overflow
--EXTENSIONS--
uri
--FILE--
<?php

$url = new \Uri\WhatWg\Url('https://example.com');

try {
    if (PHP_INT_SIZE == 8) {
        $url->withPort(9223372036854775808);
    } else {
        $url->withPort(2147483648);
    }
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Uri\WhatWg\Url::withPort(): Argument #1 ($port) must be of type ?int, float given
