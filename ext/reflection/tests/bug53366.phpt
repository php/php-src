--TEST--
Bug #53366 (Reflection doesn't get dynamic property value from getProperty())
--FILE--
<?php

#[AllowDynamicProperties]
class UserClass {
}

$myClass = new UserClass;
$myClass->id = 1000;

$reflect = new ReflectionObject($myClass);

var_dump($reflect->getProperty('id'));
var_dump($reflect->getProperty('id')->getValue($myClass));

?>
--EXPECTF--
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(2) "id"
  ["class"]=>
  string(9) "UserClass"
}
int(1000)
