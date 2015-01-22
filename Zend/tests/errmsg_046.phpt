--TEST--
Allow error handler declare referenced $errmsg parameter 01
--FILE--
<?php

set_error_handler(function($_, &$errmsg) {
	$errmsg = "[Prefix] - {$errmsg} - [Suffix]";

	return false;
});

echo $undefined;

var_dump(error_get_last());
?>
--EXPECTF--
Notice: [Prefix] - Undefined variable: undefined - [Suffix] in %s on line %d
array(4) {
  ["type"]=>
  int(8)
  ["message"]=>
  string(51) "[Prefix] - Undefined variable: undefined - [Suffix]"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(%d)
}
