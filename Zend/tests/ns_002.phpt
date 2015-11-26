--TEST--
002: Import in namespace
--FILE--
<?php
namespace test\ns1;

class Foo {
  static function bar() {
    echo __CLASS__,"\n";
  }
}

use test\ns1\Foo as Bar;
use test\ns1 as ns2;
use test\ns1;

Foo::bar();
\test\ns1\Foo::bar();
Bar::bar();
ns2\Foo::bar();
ns1\Foo::bar();
--EXPECT--
test\ns1\Foo
test\ns1\Foo
test\ns1\Foo
test\ns1\Foo
test\ns1\Foo
