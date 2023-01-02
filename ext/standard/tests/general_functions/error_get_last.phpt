--TEST--
error_get_last() tests
--FILE--
<?php

var_dump(error_get_last());
try {
    var_dump(error_get_last(true));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump(error_get_last());

$a = $b;

var_dump(error_get_last());

echo "Done\n";
?>
--EXPECTF--
NULL
error_get_last() expects exactly 0 arguments, 1 given
NULL

Warning: Undefined variable $b (this will become an error in PHP 9.0) in %s on line %d
array(4) {
  ["type"]=>
  int(2)
  ["message"]=>
  string(60) "Undefined variable $b (This will become an error in PHP 9.0)"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(11)
}
Done
