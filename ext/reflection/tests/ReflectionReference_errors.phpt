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
    ReflectionReference::fromArrayElement([], 1.5);
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

var_dump(unserialize('O:19:"ReflectionReference":0:{}'));

?>
--EXPECTF--
Call to private ReflectionReference::__construct() from invalid context
ReflectionReference::fromArrayElement() expects parameter 1 to be array, object given
Key must be array or string
Array key not found
Serialization of 'ReflectionReference' is not allowed

Warning: Erroneous data format for unserializing 'ReflectionReference' in %s on line %d

Notice: unserialize(): Error at offset 30 of 31 bytes in %s on line %d
bool(false)
