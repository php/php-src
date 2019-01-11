--TEST--
Variance checks work across same-file namespaces (brace style)
--FILE--
<?php

namespace Foo {
  class A {
    function foo(B $x): A {
      return new self();
    }
  }
}

namespace Bar {
  class B extends \Foo\A {
    function foo(\stdClass $x): B {
      return new self();
    }
  }
}

namespace {
  echo get_class((new \Bar\B())->foo(new \stdClass()));
}
?>
--EXPECTF--
Warning: Declaration of Bar\B::foo(stdClass $x): Bar\B should be compatible with Foo\A::foo(Foo\B $x): Foo\A in %s on line %d
Bar\B
