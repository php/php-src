--TEST--
Defining constants with non-scalar values
--FILE--
<?php

define('foo', new stdClass);
var_dump(foo);

define('bar', fopen(__FILE__, 'r'));
var_dump(bar);

?>
--EXPECTF--
object(stdClass)#1 (0) {
}
resource(%d) of type (stream)
