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

class FooWithDefault {
    public readonly int $bar = 1;

    public function __clone()
    {
        $this->bar++;
    }
}

$fooWithDefault = new FooWithDefault();

var_dump(clone $fooWithDefault);

$fooWithDefault2 = clone $fooWithDefault;
var_dump($fooWithDefault2);

var_dump(clone $fooWithDefault2);

class FooWithDefaultCloneWith {
    public readonly int $bar = 1;

    public function withBar(int $bar)
    {
        return clone($this, ['bar' => $bar]);
    }
}

$clone = new FooWithDefaultCloneWith();
var_dump($clone);

$clone2 = $clone->withBar(2);
var_dump($clone2);

var_dump($clone2->withBar(0));

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
object(FooWithDefault)#%d (%d) {
  ["bar"]=>
  int(2)
}
object(FooWithDefault)#%d (%d) {
  ["bar"]=>
  int(2)
}
object(FooWithDefault)#%d (%d) {
  ["bar"]=>
  int(3)
}
object(FooWithDefaultCloneWith)#%d (%d) {
  ["bar"]=>
  int(1)
}
object(FooWithDefaultCloneWith)#%d (%d) {
  ["bar"]=>
  int(2)
}
object(FooWithDefaultCloneWith)#%d (%d) {
  ["bar"]=>
  int(0)
}
