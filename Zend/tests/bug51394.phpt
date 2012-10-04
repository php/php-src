--TEST--
Bug #51394 (Error line reported incorrectly if error handler throws an exception)
--INI--
error_reporting=-1
--FILE--
<?php
function eh()
{
	throw new Exception("error!");
	return false;
}

set_error_handler("eh");
$a = $empty($b);
--EXPECTF--
Warning: Uncaught exception 'Exception' with message 'error!' in %sbug51394.php:4
Stack trace:
#0 %sbug51394.php(9): eh(8, 'Undefined varia...', '%s', 9, Array)
#1 {main}
  thrown in %sbug51394.php on line 4

Fatal error: Function name must be a string in %sbug51394.php on line 9