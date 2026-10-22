--TEST--
Various error conditions for ReflectionReference
--FILE--
<?php

try {
    new ReflectionReference();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    ReflectionReference::fromArrayElement(new stdClass, "test");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    ReflectionReference::fromArrayElement([], []);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $ary = [0, 1, 2];
    ReflectionReference::fromArrayElement($ary, 3);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $ary = [&$ary];
    $ref = ReflectionReference::fromArrayElement($ary, 0);
    var_dump(serialize($ref));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(unserialize('O:19:"ReflectionReference":0:{}'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
Error: Call to private ReflectionReference::__construct() from global scope
TypeError: ReflectionReference::fromArrayElement(): Argument #1 ($array) must be of type array, stdClass given
TypeError: ReflectionReference::fromArrayElement(): Argument #2 ($key) must be of type string|int, array given
ReflectionException: Array key not found
Exception: Serialization of 'ReflectionReference' is not allowed
Exception: Unserialization of 'ReflectionReference' is not allowed
