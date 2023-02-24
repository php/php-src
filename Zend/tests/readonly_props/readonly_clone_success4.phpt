--TEST--
Readonly property can be reset once during cloning even after a type error
--FILE--
<?php

class Foo {
    public readonly int $bar;

    public function __clone()
    {
        try {
            $this->bar = "foo";
        } catch (Error $e) {
            echo $e->getMessage() . "\n";
        }

        $this->bar = 1;
    }
}

$foo = new Foo();

var_dump(clone $foo);
var_dump(clone $foo);

?>
--EXPECTF--
Cannot assign string to property Foo::$bar of type int
object(Foo)#%d (%d) {
  ["bar"]=>
  int(1)
}
Cannot assign string to property Foo::$bar of type int
object(Foo)#%d (%d) {
  ["bar"]=>
  int(1)
}
