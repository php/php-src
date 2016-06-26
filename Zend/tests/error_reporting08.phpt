--TEST--
testing @ and error_reporting - 8
--FILE--
<?php

error_reporting(E_ALL);
	
function foo1($arg) {
}

function foo2($arg) {
}

function foo3() {
	error_reporting(E_ALL|E_STRICT);
	echo $undef3;
	throw new Exception("test");
}

try {
	@foo1(@foo2(@foo3()));
} catch (Exception $e) {
}

var_dump(error_reporting());

echo "Done\n";
?>
--EXPECTF--	
Notice: Undefined variable: undef3 in %s on line %d
int(32767)
Done
