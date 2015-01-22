--TEST--
Allow error handler declare referenced $errmsg parameter with exception
--FILE--
<?php

set_error_handler(function($_, &$errmsg) {
	$errmsg = "[Prefix] - {$errmsg} - [Suffix]";

	throw new Exception("exception from handler");

	return false;
});

echo $undefined;
?>
--EXPECTF--
Fatal error: Uncaught exception 'Exception' with message 'exception from handler' in %s:%d
Stack trace:
#0 %s(%d): {closure}(8, '[Prefix] - Unde...', '%s', %s, Array)
#1 {main}
  thrown in %s on line %d
