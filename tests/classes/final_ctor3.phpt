--TEST--
Ensure implicit final inherited old-style constructor cannot be overridden.
--FILE--
<?php
  class A {
      final function A() { }
  }
  class B extends A {
      function A() { }
  }
?>
--EXPECTF--
Fatal error: Cannot override final method A::A() in %s on line %d
