--TEST--
Allow error handler declare referenced parameters: wrong type
--FILE--
<?php

set_error_handler(function(&$code, &$errmsg, &$file, &$lineno) {
	$code   = 999;
	$errmsg = array();
	$file   = array();
	$lineno = "just test";

	return false;
});

echo $undefined;

var_dump(error_get_last());
?>
--EXPECTF--
Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Unknown error: Array in Array on line 0
array(4) {
  ["type"]=>
  int(999)
  ["message"]=>
  string(5) "Array"
  ["file"]=>
  string(5) "Array"
  ["line"]=>
  int(0)
}
