--TEST--
ReflectionExtension::getClasses();
--CREDITS--
Thijs Lensselink <tl@lenss.nl>
--FILE--
<?php
$ext = new ReflectionExtension('reflection');
var_dump($ext->getClasses());
?>
==DONE==
--EXPECTF--
array(12) {
  ["ReflectionException"]=>
  &object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(19) "ReflectionException"
  }
  ["Reflection"]=>
  &object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(10) "Reflection"
  }
  ["Reflector"]=>
  &object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(9) "Reflector"
  }
  ["ReflectionFunctionAbstract"]=>
  &object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(26) "ReflectionFunctionAbstract"
  }
  ["ReflectionFunction"]=>
  &object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(18) "ReflectionFunction"
  }
  ["ReflectionParameter"]=>
  &object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(19) "ReflectionParameter"
  }
  ["ReflectionMethod"]=>
  &object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(16) "ReflectionMethod"
  }
  ["ReflectionClass"]=>
  &object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(15) "ReflectionClass"
  }
  ["ReflectionObject"]=>
  &object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(16) "ReflectionObject"
  }
  ["ReflectionProperty"]=>
  &object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(18) "ReflectionProperty"
  }
  ["ReflectionExtension"]=>
  &object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(19) "ReflectionExtension"
  }
  ["ReflectionZendExtension"]=>
  &object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(23) "ReflectionZendExtension"
  }
}
==DONE==
