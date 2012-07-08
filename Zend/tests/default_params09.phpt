--TEST--
Default parameters - 9, varargs with +
--FILE--
<?php
$foo = $bar = "test";
var_dump(compact("foo", default, "bar"));
var_dump(compact(default, default));
echo "Done\n";
?>
--EXPECTF--	
array(2) {
  ["foo"]=>
  string(4) "test"
  ["bar"]=>
  string(4) "test"
}

Warning: compact() expects at least 1 parameter, 0 given in %s on line %d
NULL
Done

