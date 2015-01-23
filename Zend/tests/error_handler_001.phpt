--TEST--
Allow error handler declare referenced parameters
--FILE--
<?php

set_error_handler(function(&$code, &$errmsg, &$file, &$lineno) {
	$code   = E_WARNING;
	$errmsg = "[Prefix] - {$errmsg} - [Suffix]";
	$file   = "a-new-file.ext.php";
	$lineno = 999;

	return false;
});

echo $undefined;

var_dump(error_get_last());
?>
--EXPECTF--
Warning: [Prefix] - Undefined variable: undefined - [Suffix] in a-new-file.ext.php on line 999
array(4) {
  ["type"]=>
  int(2)
  ["message"]=>
  string(51) "[Prefix] - Undefined variable: undefined - [Suffix]"
  ["file"]=>
  string(%d) "a-new-file.ext.php"
  ["line"]=>
  int(999)
}
