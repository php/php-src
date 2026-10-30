--TEST--
Bug #73663.2 ("Invalid opcode 65/16/8" occurs with a variable created with list())
--FILE--
<?php
function change(&$ref) {
    $ref = range(1, 10);
    return;
}

$array = [1];
change(list($val) = $array);
var_dump($array);
?>
--EXPECTF--
Fatal error: Uncaught Error: change(): Argument #1 ($ref) could not be passed by reference in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
