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
string(129) "Return value of getNumber() must be an integer (or convertible float, convertible string or boolean), non-numeric string returned"
