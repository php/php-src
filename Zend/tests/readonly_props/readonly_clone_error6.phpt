--TEST--
Test that readonly properties cannot be reassigned by invoking the __clone() method directly
--FILE--
<?php

class Foo
{
    public function __construct(
        public readonly int $bar
    ) {}

    public function __clone()
    {
        $this->bar = 1;
    }
}

$foo = new Foo(0);

var_dump($foo);

try {
    $foo->__clone();
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $foo->__clone();
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

var_dump($foo);

?>
--EXPECTF--
object(Foo)#%d (%d) {
  ["bar"]=>
  int(0)
}
Cannot modify readonly property Foo::$bar
Cannot modify readonly property Foo::$bar
object(Foo)#%d (%d) {
  ["bar"]=>
  int(0)
}
