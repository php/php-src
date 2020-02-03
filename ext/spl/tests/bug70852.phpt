--TEST--
Bug #70852 Segfault getting NULL offset of an ArrayObject
--FILE--
<?php

$y = new ArrayObject();

var_dump($y[null]);
var_dump($y[null]++);

?>
--EXPECTF--
Notice: Undefined index:  in %s on line %d
NULL

Notice: Undefined index:  in %s on line %d
NULL
