--TEST--
ReflectionExtension::getClasses();
--CREDITS--
Thijs Lensselink <tl@lenss.nl>
--FILE--
<?php
$ext = new ReflectionExtension('reflection');
var_dump($ext->getClasses());
?>
--EXPECTF--
array(26) {
  ["ReflectionException"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(19) "ReflectionException"
  }
  ["Reflection"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(10) "Reflection"
  }
  ["Reflector"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(9) "Reflector"
  }
  ["ReflectionFunctionAbstract"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(26) "ReflectionFunctionAbstract"
  }
  ["ReflectionFunction"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(18) "ReflectionFunction"
  }
  ["ReflectionGenerator"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(19) "ReflectionGenerator"
  }
  ["ReflectionParameter"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(19) "ReflectionParameter"
  }
  ["ReflectionType"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(14) "ReflectionType"
  }
  ["ReflectionNamedType"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(19) "ReflectionNamedType"
  }
  ["ReflectionUnionType"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(19) "ReflectionUnionType"
  }
  ["ReflectionIntersectionType"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(26) "ReflectionIntersectionType"
  }
  ["ReflectionMethod"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(16) "ReflectionMethod"
  }
  ["ReflectionClass"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(15) "ReflectionClass"
  }
  ["ReflectionObject"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(16) "ReflectionObject"
  }
  ["ReflectionProperty"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(18) "ReflectionProperty"
  }
  ["ReflectionClassConstant"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(23) "ReflectionClassConstant"
  }
  ["ReflectionExtension"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(19) "ReflectionExtension"
  }
  ["ReflectionZendExtension"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(23) "ReflectionZendExtension"
  }
  ["ReflectionReference"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(19) "ReflectionReference"
  }
  ["ReflectionAttribute"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(19) "ReflectionAttribute"
  }
  ["ReflectionEnum"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(14) "ReflectionEnum"
  }
  ["ReflectionEnumUnitCase"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(22) "ReflectionEnumUnitCase"
  }
  ["ReflectionEnumBackedCase"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(24) "ReflectionEnumBackedCase"
  }
  ["ReflectionFiber"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(15) "ReflectionFiber"
  }
  ["ReflectionConstant"]=>
  object(ReflectionClass)#%d (1) {
    ["name"]=>
    string(18) "ReflectionConstant"
  }
  ["PropertyHookType"]=>
  object(ReflectionEnum)#%d (1) {
    ["name"]=>
    string(16) "PropertyHookType"
  }
}
