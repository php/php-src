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
object(stdClass)#1 (0) {
}
object(stdClass)#2 (0) {
}
object(stdClass)#3 (0) {
}
object(stdClass)#4 (0) {
}
