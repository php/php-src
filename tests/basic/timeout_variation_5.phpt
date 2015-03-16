--TEST--
Timeout within function containing exteption
--FILE--
<?php

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "timeout_config.inc";

$t = 3;
set_time_limit($t);

function f($t) { 
	echo "call";
	busy_sleep($t*2);
	throw new Exception("never reached here");
}

f($t);
?>
never reached here
--EXPECTF--
call
Fatal error: Maximum execution time of 3 seconds exceeded in %s on line %d
