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
    [%u|b%"name"]=>
    %string|unicode%(1) "B"
    [%u|b%"class"]=>
    %string|unicode%(1) "B"
  }
  [1]=>
  &object(ReflectionMethod)#%d (2) {
    [%u|b%"name"]=>
    %string|unicode%(1) "A"
    [%u|b%"class"]=>
    %string|unicode%(1) "B"
  }
}
In A::A
In A::A
In A::B