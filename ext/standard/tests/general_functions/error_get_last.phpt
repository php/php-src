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

Warning: Wrong parameter count for error_get_last() in %s on line %d
NULL
array(4) {
  ["type"]=>
  int(2)
  ["message"]=>
  string(42) "Wrong parameter count for error_get_last()"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(%d)
}

Notice: Undefined variable: b in %s on line %d
array(4) {
  ["type"]=>
  int(8)
  ["message"]=>
  string(21) "Undefined variable: b"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(%d)
}
Done
