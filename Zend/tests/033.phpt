--TEST--
Using undefined multidimensional array
--FILE--
<?php 

$arr[1][2][3][4][5];

echo $arr[1][2][3][4][5];

$arr[1][2][3][4][5]->foo;

$arr[1][2][3][4][5]->foo = 1;

$arr[][] = 2;

$arr[][]->bar = 2;

?>
--EXPECTF--

Notice: Undefined variable: arr in %s on line %d

Notice: Undefined variable: arr in %s on line %d

Notice: Undefined variable: arr in %s on line %d

Notice: Trying to get property of non-object in %s on line %d

Strict Standards: Creating default object from empty value in %s on line %d

Strict Standards: Creating default object from empty value in %s on line %d
