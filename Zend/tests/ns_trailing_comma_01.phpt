--TEST--
Mixed group use declaration can contain trailing comma
--FILE--
<?php
namespace Foo {
  const FOO_CONST = "Foo const\n";
  function foo_func() {
    echo "Foo func\n";
  }
  class FooClass {
    function __construct() {
      echo "Foo class\n";
    }
  }
}
namespace {
  use Foo\{
    const FOO_CONST,
    function foo_func,
    FooClass as BarClass,
  };
  echo FOO_CONST;
  foo_func();
  new BarClass;
}
?>
--EXPECT--
Foo const
Foo func
Foo class
