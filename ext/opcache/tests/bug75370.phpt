--TEST--
Bug #75370 (Webserver hangs on valid PHP text)
--FILE--
<?php
function test()
{
	$success = true;
	$success = $success AND true;
	return $success;
}

var_dump(test());
?>
--EXPECT--
bool(true)
