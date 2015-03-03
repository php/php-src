--TEST--
error_clear_last() tests
--FILE--
<?php

var_dump(error_get_last());

var_dump(error_clear_last());

@$a = $b;

var_dump(error_get_last());
var_dump(error_clear_last());
var_dump(error_get_last());

echo "Done\n";
?>
--EXPECTF--	
NULL
bool(false)
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
bool(true)
NULL
Done
