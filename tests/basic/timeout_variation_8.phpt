--TEST--
Timeout within foreach loop
--SKIPIF--
<?php
	if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php

include __DIR__ . DIRECTORY_SEPARATOR . "timeout_config.inc";

set_time_limit($t);

$startTime = microtime(true);

foreach (range(0, 42) as $i) {
	busy_wait(1);
}

$diff = microtime(true) - $startTime;
echo "time spent waiting: $diff\n";

?>
never reached here
--EXPECTF--
Fatal error: Maximum execution time of 3 seconds exceeded in %s on line %d
