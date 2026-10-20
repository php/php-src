--TEST--
Using undefined multidimensional array
--FILE--
<?php

$arr[1][2][3][4][5];

echo $arr[1][2][3][4][5];

$arr[1][2][3][4][5]->foo;

try {
    $arr[1][2][3][4][5]->foo = 1;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$arr[][] = 2;

try {
    $arr[][]->bar = 2;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
Warning: Undefined variable $arr in %s on line %d

Warning: Trying to access array offset on null in %s on line %d

Warning: Trying to access array offset on null in %s on line %d

Warning: Trying to access array offset on null in %s on line %d

Warning: Trying to access array offset on null in %s on line %d

Warning: Trying to access array offset on null in %s on line %d

Warning: Undefined variable $arr in %s on line %d

Warning: Trying to access array offset on null in %s on line %d

Warning: Trying to access array offset on null in %s on line %d

Warning: Trying to access array offset on null in %s on line %d

Warning: Trying to access array offset on null in %s on line %d

Warning: Trying to access array offset on null in %s on line %d

Warning: Undefined variable $arr in %s on line %d

Warning: Trying to access array offset on null in %s on line %d

Warning: Trying to access array offset on null in %s on line %d

Warning: Trying to access array offset on null in %s on line %d

Warning: Trying to access array offset on null in %s on line %d

Warning: Trying to access array offset on null in %s on line %d

Warning: Attempt to read property "foo" on null in %s on line %d
Error: Attempt to assign property "foo" on null
Error: Attempt to assign property "bar" on null
