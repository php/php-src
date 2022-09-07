--TEST--
Test that "clone with" is still chainable
--FILE--
<?php

class Foo
{
    public $bar = 1;

    public function withProperties()
    {
        return (clone $this with {})->bar;
    }
}

$foo = new Foo();
var_dump($foo);
$bar = $foo->withProperties();
var_dump($bar);

?>
--EXPECT--
object(Foo)#1 (1) {
  ["bar"]=>
  int(1)
}
int(1)
