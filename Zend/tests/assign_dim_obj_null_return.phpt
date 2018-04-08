--TEST--
Various null return conditions of dim/obj assignments
--FILE--
<?php

function test() {
    $array = [PHP_INT_MAX => 42];
    $true = true;

    var_dump($array[] = 123);
    var_dump($array[[]] = 123);
    var_dump($array[new stdClass] = 123);
    var_dump($true[123] = 456);

    var_dump($array[] += 123);
    var_dump($array[[]] += 123);
    var_dump($array[new stdClass] += 123);
    var_dump($true[123] += 456);

    var_dump($true->foo = 123);
    var_dump($true->foo += 123);
}

test();

?>
--EXPECTF--
Warning: Cannot add element to the array as the next element is already occupied in %s on line %d
NULL

Warning: Illegal offset type in %s on line %d
NULL

Warning: Illegal offset type in %s on line %d
NULL

Warning: Cannot use a scalar value as an array in %s on line %d
NULL

Warning: Cannot add element to the array as the next element is already occupied in %s on line %d
NULL

Warning: Illegal offset type in %s on line %d
NULL

Warning: Illegal offset type in %s on line %d
NULL

Warning: Cannot use a scalar value as an array in %s on line %d
NULL

Warning: Attempt to assign property 'foo' of non-object in %s on line %d
NULL

Warning: Attempt to assign property 'foo' of non-object in %s on line %d
NULL
