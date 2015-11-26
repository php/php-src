--TEST--
Timeout within shutdown function
--SKIPIF--
<?php 
	if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "timeout_config.inc";

set_time_limit($t);

function f()
{
	echo "call";
	busy_wait(4);
}

register_shutdown_function("f");
exit(0);
?>
never reached here
--EXPECTF--
call
Fatal error: Maximum execution time of 3 seconds exceeded in %s on line %d
