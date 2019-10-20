--TEST--
Timeout within function containing exception
--SKIPIF--
<?php
	if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php

include __DIR__ . DIRECTORY_SEPARATOR . "timeout_config.inc";

set_time_limit($t);

function f($t) {
	echo "call";
	$startTime = microtime(true);
	busy_wait($t*2);
	$diff = microtime(true) - $startTime;
	throw new Exception("never reached here (time spent waiting: $diff)");
}

f($t);
?>
never reached here
--EXPECTF--
call
Fatal error: Maximum execution time of 3 seconds exceeded in %s on line %d
