--TEST--
Timeout within for loop
--FILE--
<?php

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "timeout_config.inc";

$t = 3;
set_time_limit($t);

for($i = 0; $i < 42; $i++) { 
	echo 1; 
	busy_sleep(1);
}

?>
never reached here
--EXPECTF--
111
Fatal error: Maximum execution time of 3 seconds exceeded in %s on line %d
