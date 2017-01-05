--TEST--
Bug #70689 (Exception handler does not work as expected)
--FILE--
<?php

function foo($foo) {
	echo "Executing foo\n";
}

set_error_handler(function($errno, $errstr) {
	throw new Exception($errstr);
});

try {
	foo();
} catch (Exception $e) {
	echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Missing argument 1 for foo(), called in %sbug70689.php on line %d and defined
