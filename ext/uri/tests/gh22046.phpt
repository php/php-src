--TEST--
GH-22046: The unserialize function with Uri\WhatWg\Url leads to NULL pointer dereference when object serialized back
--FILE--
<?php

$payload = 'C:14:"Uri\WhatWg\Url":0:{}';
try {
    unserialize($payload);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

$payload = 'C:15:"Uri\Rfc3986\Uri":0:{}';
try {
    unserialize($payload);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Exception: Class Uri\WhatWg\Url has no unserializer
Exception: Class Uri\Rfc3986\Uri has no unserializer
