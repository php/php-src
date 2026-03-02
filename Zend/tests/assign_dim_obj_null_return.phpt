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

    try {
        var_dump($true[123] = 456);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }

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

    try {
        var_dump($true[123] += 456);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }

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
--EXPECT--
Cannot add element to the array as the next element is already occupied
Cannot access offset of type array on array
Cannot access offset of type stdClass on array
Cannot use a scalar value as an array
Cannot add element to the array as the next element is already occupied
Cannot access offset of type array on array
Cannot access offset of type stdClass on array
Cannot use a scalar value as an array
Attempt to assign property "foo" on true
Attempt to assign property "foo" on true
