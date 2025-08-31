--TEST--
Reflection Bug #30146 (ReflectionProperty->getValue() requires instance for static property)
--FILE--
<?php
class test {
  static public $a = 1;
}

$r = new ReflectionProperty('test', 'a');
var_dump($r->getValue(null));

$r->setValue(NULL, 2);
var_dump($r->getValue());

$r->setValue(3);
var_dump($r->getValue());
?>
--EXPECTF--
int(1)
int(2)

Deprecated: Calling ReflectionProperty::setValue() with a single argument is deprecated in %s on line %d
int(3)
