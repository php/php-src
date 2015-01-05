--TEST--
Error suppression for typehints on variadic arguments works
--FILE--
<?php

function test(array... $args) {
    var_dump($args);
}

set_error_handler(function($errno, $errstr) {
    var_dump($errstr);
    return true;
});

test([0], [1], 2);

?>
--EXPECTF--
string(%d) "Argument 3 passed to test() must be of the type array, integer given, called in %s on line %d and defined"
array(3) {
  [0]=>
  array(1) {
    [0]=>
    int(0)
  }
  [1]=>
  array(1) {
    [0]=>
    int(1)
  }
  [2]=>
  int(2)
}
