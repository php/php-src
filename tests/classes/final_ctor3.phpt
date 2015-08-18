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
Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; A has a deprecated constructor in %s on line %d

Fatal error: Cannot override final method A::A() in %s on line %d
