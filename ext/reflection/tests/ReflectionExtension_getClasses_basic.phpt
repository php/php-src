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
--EXPECT--
array(12) {
  ["ReflectionException"]=>
  &object(ReflectionClass)#2 (1) {
    ["name"]=>
    string(19) "ReflectionException"
  }
  ["Reflection"]=>
  &object(ReflectionClass)#3 (1) {
    ["name"]=>
    string(10) "Reflection"
  }
  ["Reflector"]=>
  &object(ReflectionClass)#4 (1) {
    ["name"]=>
    string(9) "Reflector"
  }
  ["ReflectionFunctionAbstract"]=>
  &object(ReflectionClass)#5 (1) {
    ["name"]=>
    string(26) "ReflectionFunctionAbstract"
  }
  ["ReflectionFunction"]=>
  &object(ReflectionClass)#6 (1) {
    ["name"]=>
    string(18) "ReflectionFunction"
  }
  ["ReflectionParameter"]=>
  &object(ReflectionClass)#7 (1) {
    ["name"]=>
    string(19) "ReflectionParameter"
  }
  ["ReflectionMethod"]=>
  &object(ReflectionClass)#8 (1) {
    ["name"]=>
    string(16) "ReflectionMethod"
  }
  ["ReflectionClass"]=>
  &object(ReflectionClass)#9 (1) {
    ["name"]=>
    string(15) "ReflectionClass"
  }
  ["ReflectionObject"]=>
  &object(ReflectionClass)#10 (1) {
    ["name"]=>
    string(16) "ReflectionObject"
  }
  ["ReflectionProperty"]=>
  &object(ReflectionClass)#11 (1) {
    ["name"]=>
    string(18) "ReflectionProperty"
  }
  ["ReflectionExtension"]=>
  &object(ReflectionClass)#12 (1) {
    ["name"]=>
    string(19) "ReflectionExtension"
  }
  ["ReflectionZendExtension"]=>
  &object(ReflectionClass)#13 (1) {
    ["name"]=>
    string(23) "ReflectionZendExtension"
  }
}
==DONE==
