--TEST--
Bug #46103: ReflectionObject memory leak
--FILE--
<?php

$obj = new stdClass;
$obj->r = new ReflectionObject($obj);
var_dump($obj);

?>
--EXPECT--
object(stdClass)#1 (1) {
  ["r"]=>
  object(ReflectionObject)#2 (1) {
    ["name"]=>
    string(8) "stdClass"
  }
}
