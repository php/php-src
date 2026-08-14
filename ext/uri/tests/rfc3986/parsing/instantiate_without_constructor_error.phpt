--TEST--
Test Uri\Rfc3986\Uri instantiation without calling the constructor
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

?>
--EXPECT--
ReflectionException: Class Uri\Rfc3986\Uri is an internal class marked as final that cannot be instantiated without invoking its constructor
