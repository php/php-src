--TEST--
ReflectionExtension::getClasses();
--CREDITS--
Thijs Lensselink <tl@lenss.nl>
--FILE--
<?php
$ext = new ReflectionExtension('reflection');
var_dump($ext->getClasses());
?>
--EXPECT--
array(25) {
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
  ["ReflectorWithAttributes"]=>
  object(ReflectionClass)#5 (1) {
    ["name"]=>
    string(23) "ReflectorWithAttributes"
  }
  ["ReflectionFunctionAbstract"]=>
  object(ReflectionClass)#6 (1) {
    ["name"]=>
    string(26) "ReflectionFunctionAbstract"
  }
  ["ReflectionFunction"]=>
  object(ReflectionClass)#7 (1) {
    ["name"]=>
    string(18) "ReflectionFunction"
  }
  ["ReflectionGenerator"]=>
  object(ReflectionClass)#8 (1) {
    ["name"]=>
    string(19) "ReflectionGenerator"
  }
  ["ReflectionParameter"]=>
  object(ReflectionClass)#9 (1) {
    ["name"]=>
    string(19) "ReflectionParameter"
  }
  ["ReflectionType"]=>
  object(ReflectionClass)#10 (1) {
    ["name"]=>
    string(14) "ReflectionType"
  }
  ["ReflectionNamedType"]=>
  object(ReflectionClass)#11 (1) {
    ["name"]=>
    string(19) "ReflectionNamedType"
  }
  ["ReflectionUnionType"]=>
  object(ReflectionClass)#12 (1) {
    ["name"]=>
    string(19) "ReflectionUnionType"
  }
  ["ReflectionIntersectionType"]=>
  object(ReflectionClass)#13 (1) {
    ["name"]=>
    string(26) "ReflectionIntersectionType"
  }
  ["ReflectionMethod"]=>
  object(ReflectionClass)#14 (1) {
    ["name"]=>
    string(16) "ReflectionMethod"
  }
  ["ReflectionClass"]=>
  object(ReflectionClass)#15 (1) {
    ["name"]=>
    string(15) "ReflectionClass"
  }
  ["ReflectionObject"]=>
  object(ReflectionClass)#16 (1) {
    ["name"]=>
    string(16) "ReflectionObject"
  }
  ["ReflectionProperty"]=>
  object(ReflectionClass)#17 (1) {
    ["name"]=>
    string(18) "ReflectionProperty"
  }
  ["ReflectionClassConstant"]=>
  object(ReflectionClass)#18 (1) {
    ["name"]=>
    string(23) "ReflectionClassConstant"
  }
  ["ReflectionExtension"]=>
  object(ReflectionClass)#19 (1) {
    ["name"]=>
    string(19) "ReflectionExtension"
  }
  ["ReflectionZendExtension"]=>
  object(ReflectionClass)#20 (1) {
    ["name"]=>
    string(23) "ReflectionZendExtension"
  }
  ["ReflectionReference"]=>
  object(ReflectionClass)#21 (1) {
    ["name"]=>
    string(19) "ReflectionReference"
  }
  ["ReflectionAttribute"]=>
  object(ReflectionClass)#22 (1) {
    ["name"]=>
    string(19) "ReflectionAttribute"
  }
  ["ReflectionEnum"]=>
  object(ReflectionClass)#23 (1) {
    ["name"]=>
    string(14) "ReflectionEnum"
  }
  ["ReflectionEnumUnitCase"]=>
  object(ReflectionClass)#24 (1) {
    ["name"]=>
    string(22) "ReflectionEnumUnitCase"
  }
  ["ReflectionEnumBackedCase"]=>
  object(ReflectionClass)#25 (1) {
    ["name"]=>
    string(24) "ReflectionEnumBackedCase"
  }
  ["ReflectionFiber"]=>
  object(ReflectionClass)#26 (1) {
    ["name"]=>
    string(15) "ReflectionFiber"
  }
}
