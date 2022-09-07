--TEST--
Test that properties can be assigned in "cloned with" by referencing them via a string literal expression
--FILE--
<?php

class Foo
{
    public function __construct(
        public int $bar
    ) {}

    public function with()
    {
        return clone $this with {
            "bar" => 1,
        };
    }
}

$foo = new Foo(0);

var_dump($foo->with());

?>
--EXPECTF--
object(Foo)#%d (%d) {
  ["bar"]=>
  int(1)
}