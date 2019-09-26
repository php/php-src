--TEST--
Various null return conditions of dim/obj assignments
--FILE--
<?php

function test() {
    $array = [PHP_INT_MAX => 42];
    $true = true;

    try {
        var_dump($array[] = 123);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }

    try {
        var_dump($array[[]] = 123);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }

    try {
        var_dump($array[new stdClass] = 123);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump($true[123] = 456);

    try {
        var_dump($array[] += 123);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }

    try {
        var_dump($array[[]] += 123);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }

    try {
        var_dump($array[new stdClass] += 123);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump($true[123] += 456);

    try {
        var_dump($true->foo = 123);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        var_dump($true->foo += 123);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

test();

?>
--EXPECTF--
Cannot add element to the array as the next element is already occupied
Illegal offset type
Illegal offset type

Warning: Cannot use a scalar value as an array in %s on line %d
NULL
Cannot add element to the array as the next element is already occupied
Illegal offset type
Illegal offset type

Warning: Cannot use a scalar value as an array in %s on line %d
NULL
Attempt to assign property 'foo' of non-object
Attempt to assign property 'foo' of non-object
