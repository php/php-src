--TEST--
Bug #64070 (Inheritance with traits and aliased methods)
--FILE--
<?php

trait Foo {
  public function bar() {
    echo "foo\n";
  }
}

trait Bar {
  use Foo {
    Foo::bar as foo;
  }

  public function bar() {
    echo "bar\n";
  }
}

class Boo {
  use Bar;
}

$n = new Boo;
$n->bar();
$n->foo();

?>
--EXPECT--
bar
foo
