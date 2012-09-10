--TEST--
func_get_args() can be used inside generator functions
--FILE--
<?php

function gen() {
    var_dump(func_get_args());
    yield; // trigger generator
}

$gen = gen("foo", "bar");
$gen->rewind();

?>
--EXPECT--
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
}
