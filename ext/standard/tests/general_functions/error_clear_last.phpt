--TEST--
error_clear_last() tests
--FILE--
<?php

var_dump(error_get_last());
error_clear_last();
var_dump(error_get_last());

@$a = $b;

var_dump(error_get_last());
error_clear_last();
var_dump(error_get_last());

echo "Done\n";
?>
--EXPECTF--
NULL
NULL
array(4) {
  ["type"]=>
  int(2)
  ["message"]=>
  string(60) "Undefined variable $b (this will become an error in PHP 9.0)"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(%d)
}
NULL
Done
