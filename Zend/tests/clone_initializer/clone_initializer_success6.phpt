--TEST--
Test that "clone with" works with untyped properties
--FILE--
<?php

class Foo
{
    public $bar;
    public $baz;
}

$obj1 = new Foo();
$obj2 = clone $obj1 with ["bar" => new stdClass(), "baz" => strpos("abc", "b")];
$obj3 = clone $obj2 with ["bar" => new stdClass(), "baz" => ["abc", "def"]]; // The same as above but now using cache slots

var_dump($obj1);
var_dump($obj2);
var_dump($obj3);

?>
--EXPECTF--
object(Foo)#1 (%d) {
  ["bar"]=>
  NULL
  ["baz"]=>
  NULL
}
object(Foo)#2 (%d) {
  ["bar"]=>
  object(stdClass)#3 (0) {
  }
  ["baz"]=>
  int(1)
}
object(Foo)#4 (%d) {
  ["bar"]=>
  object(stdClass)#5 (0) {
  }
  ["baz"]=>
  array(2) {
    [0]=>
    string(3) "abc"
    [1]=>
    string(3) "def"
  }
}
