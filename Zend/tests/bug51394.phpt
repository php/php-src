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
Exception: error! in %sbug51394.php on line %d
Stack trace:
#0 %sbug51394.php(%d): eh(8, 'Undefined varia%s', '%s', %d, Array)
#1 {main}
