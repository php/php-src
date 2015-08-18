--TEST--
Bug #44414 (incomplete reporting about abstract methods)
--FILE--
<?php
abstract class A {
  abstract function foo();
}
interface B {
  function bar();
}
class C extends A implements B {
}
?>
--EXPECTF--
Fatal error: Class C contains 2 abstract methods and must therefore be declared abstract or implement the remaining methods (A::foo, B::bar) in %sbug44414.php on line 8
