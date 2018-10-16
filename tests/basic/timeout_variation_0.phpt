--TEST--
Timeout within while loop
--SKIPIF--
<?php
	if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "timeout_config.inc";

set_time_limit($t);

while (1) {
	busy_wait(1);
}

?>
never reached here
--EXPECTF--
Fatal error: Maximum execution time of 3 seconds exceeded in %s on line %d
