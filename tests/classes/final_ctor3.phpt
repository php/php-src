--TEST--
Ensure implicit final inherited old-style constructor cannot be overridden.
--FILE--
<?php
  class A {
      final function A() { }
  }
  class B extends A {
  }
  class C extends B {
      function B() { }
  }
?>
--EXPECTF--
Fatal error: Cannot override final method A::B() in %s on line 9
