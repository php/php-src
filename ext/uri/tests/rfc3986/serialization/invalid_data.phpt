--TEST--
Test Uri\Rfc3986\Uri unserialization with invalid data
--FILE--
<?php

try {
    unserialize('O:15:"Uri\Rfc3986\Uri":1:{i:0;a:0:{}}'); // less than 2 items
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:15:"Uri\Rfc3986\Uri":3:{i:0;a:0:{}i:1;a:0:{}i:2;a:0:{}}'); // more than 2 items
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:15:"Uri\Rfc3986\Uri":2:{i:0;N;i:1;a:0:{}}'); // first item is not an array
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:15:"Uri\Rfc3986\Uri":2:{i:0;a:0:{}i:1;a:0:{}}'); // first array is empty
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:15:"Uri\Rfc3986\Uri":2:{i:0;a:1:{s:3:"uri";i:1;}i:1;a:0:{}}'); // "uri" key in first array is not a string
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:15:"Uri\Rfc3986\Uri":2:{i:0;a:1:{s:3:"uri";s:2:"%1";}i:1;a:0:{}}'); // "uri" key in first array contains invalid URI
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:15:"Uri\Rfc3986\Uri":2:{i:0;a:1:{s:3:"uri";s:4:"/uri";}i:1;s:0:"";}'); // second item in not an array
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:15:"Uri\Rfc3986\Uri":2:{i:0;a:1:{s:3:"uri";s:4:"/uri";}i:1;a:1:{s:5:"prop1";i:123;}}'); // second array contains a property
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Exception: Invalid serialization data for Uri\Rfc3986\Uri object
Exception: Invalid serialization data for Uri\Rfc3986\Uri object
Exception: Invalid serialization data for Uri\Rfc3986\Uri object
Exception: Invalid serialization data for Uri\Rfc3986\Uri object
Exception: Invalid serialization data for Uri\Rfc3986\Uri object
Exception: Invalid serialization data for Uri\Rfc3986\Uri object
Exception: Invalid serialization data for Uri\Rfc3986\Uri object
Exception: Invalid serialization data for Uri\Rfc3986\Uri object
