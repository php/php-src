--TEST--
Bug #73987 (Method compatibility check looks to original definition and not parent - nullability interface)
--FILE--
<?php

interface I {
  public function example($a, $b, $c);
}
class A implements I {
  public function example($a, $b = null, $c = null) { } // compatible with I::example
}
class B extends A {
  public function example($a, $b, $c = null) { } // compatible with I::example
}

?>
--EXPECTF--
Fatal error: Declaration of B::example($a, $b, $c = NULL) must be compatible with A::example($a, $b = NULL, $c = NULL) in %s
