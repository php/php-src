--TEST--
Timeout within shutdown function
--SKIPIF--
<?php
	if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php

include __DIR__ . DIRECTORY_SEPARATOR . "timeout_config.inc";

set_time_limit($t);

function f()
{
	echo "call";
	$startTime = microtime(true);
	busy_wait(5);
	$diff = microtime(true) - $startTime;
	echo "\ntime spent waiting: $diff\n";
}

register_shutdown_function("f");
exit(0);
?>
never reached here
--EXPECTF--
call
Fatal error: Maximum execution time of 3 seconds exceeded in %s on line %d
