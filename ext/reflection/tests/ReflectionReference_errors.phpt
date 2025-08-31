--TEST--
Various error conditions for ReflectionReference
--FILE--
<?php

try {
    new ReflectionReference();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    ReflectionReference::fromArrayElement(new stdClass, "test");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    ReflectionReference::fromArrayElement([], []);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $ary = [0, 1, 2];
    ReflectionReference::fromArrayElement($ary, 3);
} catch (ReflectionException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $ary = [&$ary];
    $ref = ReflectionReference::fromArrayElement($ary, 0);
    var_dump(serialize($ref));
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(unserialize('O:19:"ReflectionReference":0:{}'));
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Call to private ReflectionReference::__construct() from global scope
ReflectionReference::fromArrayElement(): Argument #1 ($array) must be of type array, stdClass given
ReflectionReference::fromArrayElement(): Argument #2 ($key) must be of type string|int, array given
Array key not found
Serialization of 'ReflectionReference' is not allowed
Unserialization of 'ReflectionReference' is not allowed
