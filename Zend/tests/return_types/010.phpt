--TEST--
Returned null, expected array reference
--FILE--
<?php
function &foo(array &$in) : array {
    return null;
}

$array = [1, 2, 3];
var_dump(foo($array));
?>
--EXPECTF--
Fatal error: Uncaught TypeError: foo(): Return value must be of type array, null returned in %s:%d
Stack trace:
#0 %s(%d): foo(Array)
#1 {main}
  thrown in %s on line %d
