--TEST--
Make sure the return value of a property assignment is not freed to early
--FILE--
<?php
class Overloaded {
    function __set($r, $a) {}
}
$obj = new Overloaded;
$x = $obj->prop = new stdClass;
var_dump($x);
?>
--EXPECT--
object(stdClass)#2 (0) {
}
