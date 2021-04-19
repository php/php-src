--TEST--
Partial application factory object properties initialization
--FILE--
<?php
class Foo {
    public function __construct(public int $arg = 1) {}
}

$foo = new Foo(...);

var_dump($foo());
?>
--EXPECTF--
object(Foo)#%d (1) {
  ["arg"]=>
  int(1)
}

