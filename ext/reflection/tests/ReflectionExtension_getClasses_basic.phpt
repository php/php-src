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
array(11) {
  [%s"ReflectionException"]=>
  &object(ReflectionClass)#2 (1) {
    [%s"name"]=>
    unicode(19) "ReflectionException"
  }
  [%s"Reflection"]=>
  &object(ReflectionClass)#3 (1) {
    [%s"name"]=>
    unicode(10) "Reflection"
  }
  [%s"Reflector"]=>
  &object(ReflectionClass)#4 (1) {
    [%s"name"]=>
    unicode(9) "Reflector"
  }
  [%s"ReflectionFunctionAbstract"]=>
  &object(ReflectionClass)#5 (1) {
    [%s"name"]=>
    unicode(26) "ReflectionFunctionAbstract"
  }
  [%s"ReflectionFunction"]=>
  &object(ReflectionClass)#6 (1) {
    [%s"name"]=>
    unicode(18) "ReflectionFunction"
  }
  [%s"ReflectionParameter"]=>
  &object(ReflectionClass)#7 (1) {
    [%s"name"]=>
    unicode(19) "ReflectionParameter"
  }
  [%s"ReflectionMethod"]=>
  &object(ReflectionClass)#8 (1) {
    [%s"name"]=>
    unicode(16) "ReflectionMethod"
  }
  [%s"ReflectionClass"]=>
  &object(ReflectionClass)#9 (1) {
    [%s"name"]=>
    unicode(15) "ReflectionClass"
  }
  [%s"ReflectionObject"]=>
  &object(ReflectionClass)#10 (1) {
    [%s"name"]=>
    unicode(16) "ReflectionObject"
  }
  [%s"ReflectionProperty"]=>
  &object(ReflectionClass)#11 (1) {
    [%s"name"]=>
    unicode(18) "ReflectionProperty"
  }
  [%s"ReflectionExtension"]=>
  &object(ReflectionClass)#12 (1) {
    [%s"name"]=>
    unicode(19) "ReflectionExtension"
  }
}
==DONE==
