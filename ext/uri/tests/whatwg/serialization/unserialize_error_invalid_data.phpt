--TEST--
Test Uri\WhatWg\Url unserialize() - error - invalid data
--FILE--
<?php

try {
    unserialize('O:14:"Uri\WhatWg\Url":1:{i:0;a:0:{}}'); // less than 2 items
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:14:"Uri\WhatWg\Url":3:{i:0;a:0:{}i:1;a:0:{}i:2;a:0:{}}'); // more than 2 items
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:14:"Uri\WhatWg\Url":2:{i:0;N;i:1;a:0:{}}'); // first item is not an array
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:14:"Uri\WhatWg\Url":2:{i:0;a:0:{}i:1;a:0:{}}'); // first array is empty
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:14:"Uri\WhatWg\Url":2:{i:0;a:2:{s:3:"uri";s:19:"https://example.com";s:1:"a";i:1;}i:1;a:0:{}}'); // "uri" key in first array contains more than 1 item
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:14:"Uri\WhatWg\Url":2:{i:0;a:1:{s:3:"uri";i:1;}i:1;a:0:{}}'); // "uri" key in first array is not a string
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:14:"Uri\WhatWg\Url":2:{i:0;a:1:{s:3:"uri";s:11:"invalid-url";}i:1;a:0:{}}'); // "uri" key in first array contains invalid URL
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:14:"Uri\WhatWg\Url":2:{i:0;a:1:{s:3:"uri";s:19:"https://example.com";}i:1;s:0:"";}'); // second item in not an array
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:14:"Uri\WhatWg\Url":2:{i:0;a:1:{s:3:"uri";s:19:"https://example.com";}i:1;a:1:{s:5:"prop1";i:123;}}'); // second array contains property
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Exception: Invalid serialization data for Uri\WhatWg\Url object
Exception: Invalid serialization data for Uri\WhatWg\Url object
Exception: Invalid serialization data for Uri\WhatWg\Url object
Exception: Invalid serialization data for Uri\WhatWg\Url object
Exception: Invalid serialization data for Uri\WhatWg\Url object
Exception: Invalid serialization data for Uri\WhatWg\Url object
Exception: Invalid serialization data for Uri\WhatWg\Url object
Exception: Invalid serialization data for Uri\WhatWg\Url object
Exception: Invalid serialization data for Uri\WhatWg\Url object
