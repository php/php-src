--TEST--
Using undefined multidimensional array
--FILE--
<?php

$arr[1][2][3][4][5];

echo $arr[1][2][3][4][5];

$arr[1][2][3][4][5]->foo;

try {
    $arr[1][2][3][4][5]->foo = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$arr[][] = 2;

try {
    $arr[][]->bar = 2;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
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

Warning: Undefined variable $arr in %s on line %d

Warning: Trying to access array offset on null in %s on line %d

Warning: Trying to access array offset on null in %s on line %d

Warning: Trying to access array offset on null in %s on line %d

Warning: Trying to access array offset on null in %s on line %d

Warning: Trying to access array offset on null in %s on line %d
Attempt to assign property "foo" on null
Cannot use [] for reading
