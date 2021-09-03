--TEST--
Const array deference
--FILE--
<?php
error_reporting(E_ALL);

var_dump([1, 2, 3, 4,][3]);
var_dump([1, 2, 3, 4]['foo']);
var_dump([array(1,2,3), [4, 5, 6]][1][2]);

foreach (array([1, 2, 3])[0] as $var) {
     echo $var;
}
?>
--EXPECTF--
int(4)

Warning: Undefined array key "foo" in %s on line %d
NULL
int(6)
123
