--TEST--
Timeout within shutdown function
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
exit(0);
?>
never reached here
--EXPECTF--
call
Fatal error: Maximum execution time of 3 seconds exceeded in %s on line %d
