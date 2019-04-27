--TEST--
Bug #70557 (Memleak on return type verifying failed).
--INI--
opcache.enable=0
--FILE--
<?php

function getNumber() : int {
	return "foo";
}

try {
	getNumber();
} catch (TypeError $e) {
	var_dump($e->getMessage());
}
?>
--EXPECT--
string(68) "Return value of getNumber() must be of the type int, string returned"
