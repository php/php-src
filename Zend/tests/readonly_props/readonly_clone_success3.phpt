--TEST--
Test that __clone() unset properties
--FILE--
<?php

class Foo {
    public function __construct(
        public readonly stdClass $bar,
    ) {}

    public function __clone()
    {
        unset($this->bar);
    }
}

$foo = new Foo(new stdClass());
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
