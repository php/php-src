--TEST--
Objects in constant expressions
--FILE--
<?php

class Foo {
    public function __construct(public int $prop) {}
}

class Test {
    const CONSTANT = new Foo(0);
    public static $staticProp = new Foo(1);
    public $prop = new Foo(2);
}

const CONSTANT = new Foo(3);

function test($arg = new Foo(4)) {
    return $arg;
}

var_dump(Test::CONSTANT);
var_dump(Test::$staticProp);
var_dump((new Test)->prop);
var_dump(CONSTANT);
var_dump(test());

?>
--EXPECT--
object(Foo)#2 (1) {
  ["prop"]=>
  int(0)
}
object(Foo)#3 (1) {
  ["prop"]=>
  int(1)
}
object(Foo)#5 (1) {
  ["prop"]=>
  int(2)
}
object(Foo)#1 (1) {
  ["prop"]=>
  int(3)
}
object(Foo)#5 (1) {
  ["prop"]=>
  int(4)
}
