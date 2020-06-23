--TEST--
Bug #79487 (::getStaticProperties() ignores property modifications)
--FILE--
<?php
class Foo {
    public static $bar = 'orig';
}

Foo::$bar = 'new';
$rc = new ReflectionClass('Foo');
var_dump($rc->getStaticProperties());

class A {
  public static $a = 'A old';
}
class B extends A {
  public static $b = 'B old';
}

$rc = new ReflectionClass(B::class);
A::$a = 'A new';
var_dump($rc->getStaticProperties());
?>
--EXPECT--
array(1) {
  ["bar"]=>
  string(3) "new"
}
array(2) {
  ["b"]=>
  string(5) "B old"
  ["a"]=>
  string(5) "A new"
}
