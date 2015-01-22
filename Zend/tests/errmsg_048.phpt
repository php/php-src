--TEST--
Allow error handler declare referenced $errmsg parameter with wrong data type
--FILE--
<?php

set_error_handler(function($_, &$errmsg) {
	$errmsg = array();

	return false;
});

echo $undefined;
?>
--EXPECTF--
Notice: Array to string conversion in %s on line %d

Notice: Array in %s on line %d
