--TEST--
"Nested" list()
--FILE--
<?php

list($a, list($b)) = array(new stdclass, array(new stdclass));
var_dump($a, $b);
[$a, [$b]] = array(new stdclass, array(new stdclass));
var_dump($a, $b);

?>
--EXPECT--
object(DynamicObject)#1 (0) {
}
object(DynamicObject)#2 (0) {
}
object(DynamicObject)#3 (0) {
}
object(DynamicObject)#4 (0) {
}
