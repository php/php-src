--TEST--
Timeout within shutdown function, variation
--FILE--
<?php

$t = 3;
set_time_limit($t);

function f()
{
	echo "call";
	sleep(4);
}

register_shutdown_function("f");
?>
shutdown happens after here
--EXPECTF--
shutdown happens after here
call
Fatal error: Maximum execution time of 3 seconds exceeded in %s on line %d
