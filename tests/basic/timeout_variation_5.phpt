--TEST--
Timeout within function containing exteption
--FILE--
<?php

$t = 3;
set_time_limit($t);

function f($t) { 
	echo "call";
	sleep($t*2);
	throw new Exception("never reached here");
}

f($t);
?>
never reached here
--EXPECTF--
call
Fatal error: Maximum execution time of 3 seconds exceeded in %s on line %d
