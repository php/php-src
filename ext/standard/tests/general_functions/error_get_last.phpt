--TEST--
error_get_last() tests
--FILE--
<?php

var_dump(error_get_last());
var_dump(error_get_last(true));
var_dump(error_get_last());

$a = $b;

var_dump(error_get_last());

echo "Done\n";
?>
--EXPECTF--	
NULL

Warning: error_get_last() expects exactly 0 parameters, 1 given in %s on line %d
NULL
array(4) {
  ["type"]=>
  int(2)
  ["message"]=>
  string(54) "error_get_last() expects exactly 0 parameters, 1 given"
  ["file"]=>
  string(%i) "%s"
  ["line"]=>
  int(4)
}

Notice: Undefined variable: b in %s on line %d
array(4) {
  ["type"]=>
  int(8)
  ["message"]=>
  string(21) "Undefined variable: b"
  ["file"]=>
  string(%i) "%s"
  ["line"]=>
  int(7)
}
Done
