--TEST--
Unmixed group use declaration can contain trailing comma
--FILE--
<?php
namespace Foo {
  const FOO_CONST_1 = "Foo const 1\n";
  const FOO_CONST_2 = "Foo const 2\n";
}
namespace Bar {
  function foo_func_1() {
    echo "Bar func 1\n";
  }
  function foo_func_2() {
    echo "Bar func 2\n";
  }
}
namespace Baz {
  class BazFooClass {
    function __construct() { echo "BazFoo class\n"; }
  }
  class BazBarClass {
    function __construct() { echo "BazBar class\n"; }
  }
}
namespace {
  use const Foo\{
    FOO_CONST_1,
    FOO_CONST_2,
  };
  use function Bar\{
    foo_func_1,
    foo_func_2,
  };
  use Baz\{
    BazFooClass,
    BazBarClass,
  };
  echo FOO_CONST_1;
  echo FOO_CONST_2;
  foo_func_1();
  foo_func_2();
  new BazFooClass;
  new BazBarClass;
}
?>
--EXPECT--
Foo const 1
Foo const 2
Bar func 1
Bar func 2
BazFoo class
BazBar class
