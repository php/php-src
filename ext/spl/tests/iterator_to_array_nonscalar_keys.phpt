--TEST--
Tests iterator_to_array() with non-scalar keys
--FILE--
<?php

function gen() {
    yield "foo" => 0;
    yield 1     => 1;
    yield 2.5   => 2;
    yield null  => 3;
    yield []    => 4;
    yield new stdClass => 5;
}

var_dump(iterator_to_array(gen()));

?>
--EXPECTF--
Warning: Illegal offset type in %s on line %d

Warning: Illegal offset type in %s on line %d
array(4) {
  ["foo"]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [""]=>
  int(3)
}
