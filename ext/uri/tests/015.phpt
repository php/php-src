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
} catch (ReflectionException $e) {
    echo $e->getMessage() . "\n";
}

try {
    $reflectionClass = new ReflectionClass(Uri\WhatWg\Url::class);
    $reflectionClass->newInstanceWithoutConstructor();
} catch (ReflectionException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Class Uri\Rfc3986\Uri is an internal class marked as final that cannot be instantiated without invoking its constructor
Class Uri\WhatWg\Url is an internal class marked as final that cannot be instantiated without invoking its constructor
