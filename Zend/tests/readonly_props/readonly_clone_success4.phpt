--TEST--
Readonly property can be reset once during cloning even after a type error
--FILE--
<?php

class Foo {
    public function __construct(
        public readonly int $bar
    ) {}

    public function __clone()
    {
        try {
            $this->bar = "foo";
        } catch (Error $e) {
            echo $e->getMessage() . "\n";
        }

        $this->bar = 2;
    }
}

$foo = new Foo(1);

var_dump(clone $foo);
var_dump(clone $foo);

?>
--EXPECTF--
Cannot assign string to property Foo::$bar of type int
object(Foo)#%d (%d) {
  ["bar"]=>
  int(2)
}
Cannot assign string to property Foo::$bar of type int
object(Foo)#%d (%d) {
  ["bar"]=>
  int(2)
}
