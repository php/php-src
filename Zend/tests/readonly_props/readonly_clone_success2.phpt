--TEST--
Test that __clone() unset and reassign properties
--FILE--
<?php

class Foo {
    public function __construct(
        public readonly stdClass $bar,
    ) {}

    public function __clone()
    {
        unset($this->bar);
        var_dump($this);
        $this->bar = new stdClass();
    }
}

$foo = new Foo(new stdClass());
var_dump($foo);
$foo2 = clone $foo;

var_dump($foo);
var_dump($foo2);

?>
--EXPECTF--
object(Foo)#1 (%d) {
  ["bar"]=>
  object(stdClass)#2 (%d) {
  }
}
object(Foo)#3 (%d) {
  ["bar"]=>
  uninitialized(stdClass)
}
object(Foo)#1 (%d) {
  ["bar"]=>
  object(stdClass)#2 (%d) {
  }
}
object(Foo)#3 (%d) {
  ["bar"]=>
  object(stdClass)#4 (%d) {
  }
}
