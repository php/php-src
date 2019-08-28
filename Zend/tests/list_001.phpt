--TEST--
"Nested" list()
--FILE--
<?php

list($a, list($b)) = array(new StdClass, array(new StdClass));
var_dump($a, $b);
[$a, [$b]] = array(new StdClass, array(new StdClass));
var_dump($a, $b);

?>
--EXPECT--
object(StdClass)#1 (0) {
}
object(StdClass)#2 (0) {
}
object(StdClass)#3 (0) {
}
object(StdClass)#4 (0) {
}
