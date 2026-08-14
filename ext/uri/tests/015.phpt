--TEST--
Test instantiation without calling constructor
--EXTENSIONS--
reflection
uri
--FILE--
<?php

try {
    $reflectionClass = new ReflectionClass(Uri\Rfc3986\Uri::class);
    $reflectionClass->newInstanceWithoutConstructor();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $reflectionClass = new ReflectionClass(Uri\WhatWg\Url::class);
    $reflectionClass->newInstanceWithoutConstructor();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ReflectionException: Class Uri\Rfc3986\Uri is an internal class marked as final that cannot be instantiated without invoking its constructor
ReflectionException: Class Uri\WhatWg\Url is an internal class marked as final that cannot be instantiated without invoking its constructor
