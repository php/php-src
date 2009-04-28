--TEST--
Ensure inherited old-style constructor doesn't block other methods.
--FILE--
<?php
class A {
  public function B () { echo "In " . __METHOD__ . "\n"; }
  public function A () { echo "In " . __METHOD__ . "\n"; }
}
class B extends A { }

$rc = new ReflectionClass('B');
var_dump($rc->getMethods());


$b = new B();
$b->a();
$b->b();

?>
--EXPECTF--
array(2) {
  [0]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(1) "B"
    [u"class"]=>
    unicode(1) "A"
  }
  [1]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(1) "A"
    [u"class"]=>
    unicode(1) "A"
  }
}
In A::A
In A::A
In A::B
