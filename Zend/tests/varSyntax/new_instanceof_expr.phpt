--TEST--
new with an arbitrary expression
--FILE--
<?php

$class = 'class';
var_dump(new ('std'.$class));
var_dump(new ('std'.$class)());
$obj = new stdClass;
var_dump($obj instanceof ('std'.$class));

?>
--EXPECT--
object(stdClass)#1 (0) {
}
object(stdClass)#1 (0) {
}
bool(true)
