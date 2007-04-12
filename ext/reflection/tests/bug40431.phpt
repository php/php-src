--TEST--
Bug #40431 (dynamic properties may cause crash in ReflectionProperty methods)
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php

echo "=== 1st test ===\n";

$Obj->value = 'value';
$RefObj = new ReflectionObject($Obj);

$props = $RefObj->getProperties();

var_dump($props);
var_dump($props[0]->isStatic());
var_dump($props[0]->isPrivate());
var_dump($props[0]->isPublic());
var_dump($props[0]->isProtected());

echo "=== 2nd test ===\n";

class test1 {
}

class test2 extends test1{
}

$Obj = new test2;
$Obj->value = 'value';
$RefObj = new ReflectionObject($Obj);

$props = $RefObj->getProperties();

var_dump($props);
var_dump($props[0]->isStatic());
var_dump($props[0]->isPrivate());
var_dump($props[0]->isPublic());
var_dump($props[0]->isProtected());

echo "=== 3rd test ===\n";

class test3 {
}

$Obj = new test3;
$Obj->value = 'value';
$RefObj = new ReflectionObject($Obj);

$props = $RefObj->getProperties();

var_dump($props);
var_dump($props[0]->isStatic());
var_dump($props[0]->isPrivate());
var_dump($props[0]->isPublic());
var_dump($props[0]->isProtected());

echo "=== 4th test ===\n";

class test5 {
	private $value = 1;
}

class test4 extends test5{
}

$Obj = new test4;
$Obj->value = 'value';
$RefObj = new ReflectionObject($Obj);

$props = $RefObj->getProperties();

var_dump($props);
var_dump($props[0]->isStatic());
var_dump($props[0]->isPrivate());
var_dump($props[0]->isPublic());
var_dump($props[0]->isProtected());

echo "Done\n";
?>
--EXPECTF--	
=== 1st test ===

Strict Standards: Creating default object from empty value in %s on line %d
array(1) {
  [0]=>
  &object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(5) "value"
    ["class"]=>
    string(8) "stdClass"
  }
}
bool(false)
bool(false)
bool(true)
bool(false)
=== 2nd test ===
array(1) {
  [0]=>
  &object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(5) "value"
    ["class"]=>
    string(5) "test2"
  }
}
bool(false)
bool(false)
bool(true)
bool(false)
=== 3rd test ===
array(1) {
  [0]=>
  &object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(5) "value"
    ["class"]=>
    string(5) "test3"
  }
}
bool(false)
bool(false)
bool(true)
bool(false)
=== 4th test ===
array(1) {
  [0]=>
  &object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(5) "value"
    ["class"]=>
    string(5) "test4"
  }
}
bool(false)
bool(false)
bool(true)
bool(false)
Done
