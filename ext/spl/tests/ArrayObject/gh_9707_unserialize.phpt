--TEST--
GH-9707: object_properties_load crashes in debug mode when unserializing references to typed properties in php 8.1+
--FILE--
<?php

class Foo extends ArrayObject {
    public int $a = 0;
    public int $b = 0;
}

$f = new Foo();
$r = &$f->a;
$f->b = &$r;

$f->b = 1;
var_dump($unserialized = unserialize(serialize($f)));

$unserialized->b = 2;

var_dump($unserialized);

?>
--EXPECTF--
object(Foo)#%d (3) {
  ["a"]=>
  &int(1)
  ["b"]=>
  &int(1)
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
object(Foo)#%d (3) {
  ["a"]=>
  &int(2)
  ["b"]=>
  &int(2)
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
