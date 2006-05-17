--TEST--
testing @ and error_reporting - 1
--FILE--
<?php

error_reporting(E_ALL);

function foo($arg) {
}

function bar() {
	throw new Exception("test");
}
	
try {
	@foo(@bar());
} catch (Exception $e) {
}

var_dump(error_reporting());

echo "Done\n";
?>
--EXPECT--	
int(6143)
Done
