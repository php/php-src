--FILE--
Closure must not leak during a dynmaic call interrupted by an exception
--TEST--
<?php

(function() {
	$closure = function($foo) { var_dump($foo); };
	$closure(yield);
})()->valid(); // start

?>
==DONE==
--EXPECT--
==DONE==
