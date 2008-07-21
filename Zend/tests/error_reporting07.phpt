--TEST--
testing @ and error_reporting - 7
--FILE--
<?php

error_reporting(E_ALL);
	
function foo1($arg) {
}

function foo2($arg) {
}

function foo3($arg) {
	echo $undef3;
	throw new Exception("test");
}

try {
	@error_reporting(@foo1(@foo2(@foo3())));
} catch (Exception $e) {
}

var_dump(error_reporting());

echo "Done\n";
?>
--EXPECTF--	
int(30719)
Done
