--TEST--
Delayed variance checks work with conditional statements
--FILE--
<?php

interface A {
  static function m(Y $z): A;
}

if (true) {
  class B implements A {
    static function m(X $z): B {
      return new self();
    }
  }
  class X {}
  //class Y extends X {}
}

echo get_class(B::m(new X()));
?>
--EXPECTF--
Fatal error: Declaration of B::m(X $z): B must be compatible with A::m(Y $z): A in %s on line %d
