--TEST--
Bug #46103: ReflectionObject memory leak
--FILE--
<?php

$obj = new StdClass;
$obj->r = new ReflectionObject($obj);
var_dump($obj);

?>
--EXPECT--
object(StdClass)#1 (1) {
  ["r"]=>
  object(ReflectionObject)#2 (1) {
    ["name"]=>
    string(8) "StdClass"
  }
}
