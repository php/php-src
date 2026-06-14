--TEST--
Bug #73987 (Method compatibility check looks to original definition and not parent - return types interface)
--FILE--
<?php

interface I {
  public function example();
}
class A implements I {
  public function example(): int { } // compatible with I::example
}
class B extends A {
  public function example(): string { } // compatible with I::example
}

?>
--EXPECTF--
Fatal error: Declaration of B::example(): string must be compatible with A::example(): int in %s
