--TEST--
Using undefined multidimensional array
--FILE--
<?php

$arr[1][2][3][4][5];

echo $arr[1][2][3][4][5];

$arr[][] = 2;

?>
--EXPECTF--
Notice: Undefined variable: arr in %s on line %d

Notice: Undefined variable: arr in %s on line %d
