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
Fatal error: Function name must be a string in %sbug51394.php on line 9