--TEST--
Reflection Bug #30146 (ReflectionProperty->getValue() requires instance for static property)
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
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
===DONE===
--EXPECT--
int(1)
int(2)
int(3)
===DONE===