--TEST--
Bug #73960: Leak with instance method calling static method with referenced return
--FILE--
<?php

$array = array('one');
$array = $ref =& $array;
var_dump($array);

?>
--EXPECT--
array(1) {
  [0]=>
  string(3) "one"
}
