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
object(DynamicObject)#1 (0) {
}
object(DynamicObject)#1 (0) {
}
bool(true)
