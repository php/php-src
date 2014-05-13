--TEST--
Closure 017: Trying to destroy an active lambda function
--FILE--
<?php

$a = function(&$a) { $a = 1; };

$a($a);

?>
--EXPECTF--
Fatal error: Cannot destroy active lambda function in %s on line %d
