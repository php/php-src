--TEST--
testing @ and error_reporting - 2
--FILE--
<?php

error_reporting(E_ALL & ~E_DEPRECATED);

function foo($arg) {
}

function bar() {
	error_reporting(E_ALL);
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
int(32767)
Done
