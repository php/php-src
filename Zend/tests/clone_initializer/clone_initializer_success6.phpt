--TEST--
Test that the "clone with" works with expressions as property values
--FILE--
<?php

class Foo
{
    public $bar;
    public $baz;
}

$foo = new Foo();
$foo = clone $foo with {bar: new stdClass(), baz: strpos("abc", "b")};
var_dump($foo);

?>
--EXPECT--
object(Foo)#2 (2) {
  ["bar"]=>
  object(stdClass)#3 (0) {
  }
  ["baz"]=>
  int(1)
}
