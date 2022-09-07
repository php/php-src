--TEST--
Test that "clone with" works with readonly properties
--FILE--
<?php

class Foo
{
    public function __construct(
        public readonly int $bar,
        public readonly stdClass $baz,
    ) {}

    public function with(int $bar, stdClass $baz)
    {
        return clone $this with {
            bar: $bar,
            baz: $baz,
        };
    }
}

$stdClass = new stdClass();

$obj1 = new Foo(0, $stdClass);
$obj2 = $obj1->with(1, $stdClass);
$obj3 = $obj2->with(2, new stdClass()); // The same as above but now using cache slots

var_dump($obj1);
var_dump($obj2);
var_dump($obj3);

?>
--EXPECTF--
object(Foo)#2 (%d) {
  ["bar"]=>
  int(0)
  ["baz"]=>
  object(stdClass)#1 (%d) {
  }
}
object(Foo)#3 (%d) {
  ["bar"]=>
  int(1)
  ["baz"]=>
  object(stdClass)#1 (%d) {
  }
}
object(Foo)#5 (%d) {
  ["bar"]=>
  int(2)
  ["baz"]=>
  object(stdClass)#4 (%d) {
  }
}
