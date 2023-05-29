--TEST--
Test that readonly properties can be assigned in "cloned with" even if they were assigned in the __clone() method
--FILE--
<?php

class Foo
{
    public function __construct(
        public int $bar
    ) {}

    public function __clone()
    {
        $this->bar = 1;
    }

    public function with()
    {
        return clone $this with [
            "bar" => 2,
        ];
    }
}

$foo = new Foo(0);

var_dump($foo->with());

?>
--EXPECTF--
object(Foo)#%d (%d) {
  ["bar"]=>
  int(2)
}
