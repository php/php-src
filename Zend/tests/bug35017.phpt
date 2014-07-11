--TEST--
Bug #35017 (Exception thrown in error handler may cause unexpected behavior)
--FILE--
<?php
set_error_handler('errorHandler');
try {
	if ($a) {
		echo "1\n";
	} else {
		echo "0\n";
	}
	echo "?\n";
} catch(Exception $e) { 
  echo "This Exception should be catched\n";
}
function errorHandler($errno, $errstr, $errfile, $errline, $vars) {
	throw new Exception('Some Exception');
}
?>
--EXPECT--
This Exception should be catched
