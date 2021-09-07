--TEST--
Testing full-reference on list()
--FILE--
<?php

error_reporting(E_ALL);

$a = new stdclass;
$b =& $a;

list($a, list($b)) = array($a, array($b));
var_dump($a, $b, $a === $b);

?>
--EXPECT--
object(DynamicObject)#1 (0) {
}
object(DynamicObject)#1 (0) {
}
bool(true)
