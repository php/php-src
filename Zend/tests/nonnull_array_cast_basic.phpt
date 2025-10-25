--TEST--
Non-null array cast: basic functionality
--FILE--
<?php

var_dump((!array) [1, 2, 3]);

class Foo {
    public $bar = "baz";
}
var_dump((!array) new Foo());

?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(1) {
  ["bar"]=>
  string(3) "baz"
}
