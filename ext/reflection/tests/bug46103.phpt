--TEST--
Bug #46103: ReflectionObject memory leak
--FILE--
<?php

$obj = new stdClass;
$obj->r = new ReflectionObject($obj);
var_dump($obj);

?>
--EXPECT--
object(DynamicObject)#1 (1) {
  ["r"]=>
  object(ReflectionObject)#2 (1) {
    ["name"]=>
    string(13) "DynamicObject"
  }
}
