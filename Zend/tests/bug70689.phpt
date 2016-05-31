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
Fatal error: Uncaught Error: Too few arguments to function foo(), 0 passed in %sbug70689.php on line 12 and exactly 1 expected in %sbug70689.php:3
Stack trace:
#0 %sbug70689.php(12): foo()
#1 {main}
  thrown in %sbug70689.php on line 3
