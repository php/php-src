--TEST--
Bug #73900: Use After Free in unserialize() SplFixedArray
--FILE--
<?php

$a = new stdClass;
$b = new SplFixedArray(1);
$b[0] = $a;
$c = &$b[0];
var_dump($c);

?>
--EXPECT--
object(DynamicObject)#1 (0) {
}
