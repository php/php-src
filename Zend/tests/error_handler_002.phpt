--TEST--
Allow error handler declare referenced parameters: with exception
--FILE--
<?php

set_error_handler(function(&$code, &$errmsg, &$file, &$lineno) {
	$code   = E_WARNING;
	$errmsg = "[Prefix] - {$errmsg} - [Suffix]";
	$file   = "a-new-file.ext.php";
	$lineno = 999;

	throw new Exception("exception from handler");
	return false;
});

echo $undefined;

var_dump(error_get_last());
?>
--EXPECTF--
Fatal error: Uncaught exception 'Exception' with message 'exception from handler' in %s:%d
Stack trace:
#0 %s(%d): {closure}(2, '[Prefix] - Unde...', 'a-new-file.ext....', 999, Array)
#1 {main}
  thrown in %s on line %d
