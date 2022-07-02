--TEST--
002: Import - different syntaxes
--FILE--
<?php
namespace test\ns1;

class Foo {
  static function bar() {
    echo __CLASS__,"\n";
  }
}

class Foo2 {
  static function bar() {
    echo __CLASS__,"\n";
  }
}

namespace xyz;
use test\ns1\Foo;
use test\ns1\Foo as Bar;
use \test\ns1\Foo2;
use \test\ns1\Foo2 as Bar2;

Foo::bar();
Bar::bar();
Foo2::bar();
Bar2::bar();
?>
--EXPECT--
test\ns1\Foo
test\ns1\Foo
test\ns1\Foo2
test\ns1\Foo2
