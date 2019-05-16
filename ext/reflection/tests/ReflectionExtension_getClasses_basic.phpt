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
array(17) {
  ["ReflectionException"]=>
  object(ReflectionClass)#2 (1) {
    ["name"]=>
    string(19) "ReflectionException"
  }
  ["Reflection"]=>
  object(ReflectionClass)#3 (1) {
    ["name"]=>
    string(10) "Reflection"
  }
  ["Reflector"]=>
  object(ReflectionClass)#4 (1) {
    ["name"]=>
    string(9) "Reflector"
  }
  ["ReflectionFunctionAbstract"]=>
  object(ReflectionClass)#5 (1) {
    ["name"]=>
    string(26) "ReflectionFunctionAbstract"
  }
  ["ReflectionFunction"]=>
  object(ReflectionClass)#6 (1) {
    ["name"]=>
    string(18) "ReflectionFunction"
  }
  ["ReflectionGenerator"]=>
  object(ReflectionClass)#7 (1) {
    ["name"]=>
    string(19) "ReflectionGenerator"
  }
  ["ReflectionParameter"]=>
  object(ReflectionClass)#8 (1) {
    ["name"]=>
    string(19) "ReflectionParameter"
  }
  ["ReflectionType"]=>
  object(ReflectionClass)#9 (1) {
    ["name"]=>
    string(14) "ReflectionType"
  }
  ["ReflectionNamedType"]=>
  object(ReflectionClass)#10 (1) {
    ["name"]=>
    string(19) "ReflectionNamedType"
  }
  ["ReflectionMethod"]=>
  object(ReflectionClass)#11 (1) {
    ["name"]=>
    string(16) "ReflectionMethod"
  }
  ["ReflectionClass"]=>
  object(ReflectionClass)#12 (1) {
    ["name"]=>
    string(15) "ReflectionClass"
  }
  ["ReflectionObject"]=>
  object(ReflectionClass)#13 (1) {
    ["name"]=>
    string(16) "ReflectionObject"
  }
  ["ReflectionProperty"]=>
  object(ReflectionClass)#14 (1) {
    ["name"]=>
    string(18) "ReflectionProperty"
  }
  ["ReflectionClassConstant"]=>
  object(ReflectionClass)#15 (1) {
    ["name"]=>
    string(23) "ReflectionClassConstant"
  }
  ["ReflectionExtension"]=>
  object(ReflectionClass)#16 (1) {
    ["name"]=>
    string(19) "ReflectionExtension"
  }
  ["ReflectionZendExtension"]=>
  object(ReflectionClass)#17 (1) {
    ["name"]=>
    string(23) "ReflectionZendExtension"
  }
  ["ReflectionReference"]=>
  object(ReflectionClass)#18 (1) {
    ["name"]=>
    string(19) "ReflectionReference"
  }
}
==DONE==
