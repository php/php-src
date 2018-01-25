--TEST--
Timeout within shutdown function, variation
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
	busy_wait(5);
}

register_shutdown_function("f");
?>
shutdown happens after here
--EXPECTF--
shutdown happens after here
call
Fatal error: Maximum execution time of 3 seconds exceeded in %s on line %d
