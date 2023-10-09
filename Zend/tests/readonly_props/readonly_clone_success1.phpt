--TEST--
Readonly property can be reset once during cloning
--FILE--
<?php

class Foo {
    public function __construct(
        public readonly int $bar
    ) {}

    public function __clone()
    {
        $this->bar++;
    }
}

$foo = new Foo(1);

var_dump(clone $foo);

$foo2 = clone $foo;
var_dump($foo2);

var_dump(clone $foo2);

?>
--EXPECTF--
object(Foo)#%d (%d) {
  ["bar"]=>
  int(2)
}
object(Foo)#%d (%d) {
  ["bar"]=>
  int(2)
}
object(Foo)#%d (%d) {
  ["bar"]=>
  int(3)
}
