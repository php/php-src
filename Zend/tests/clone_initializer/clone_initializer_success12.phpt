--TEST--
Test that non-readonly properties can be assigned in "cloned with" multiple times
--FILE--
<?php

class Foo
{
    public function __construct(
        public int $bar
    ) {}

    public function with()
    {
        return clone $this with [
            "bar" => 1,
            "bar" => 2,
            "bar" => 3,
        ];
    }
}

$foo = new Foo(0);

var_dump($foo->with());

?>
--EXPECTF--
object(Foo)#%d (%d) {
  ["bar"]=>
  int(3)
}