--TEST--
Closure 017: Trying to destroy an active lambda function
--FILE--
<?php

$a = function(&$a) { $a = 1; };

$a($a);

?>
DONE
--EXPECT--
DONE
