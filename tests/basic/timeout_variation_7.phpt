--TEST--
Timeout within for loop
--FILE--
<?php

$t = 3;
set_time_limit($t);

for($i = 0; $i < 42; $i++) { 
	echo 1; 
	sleep(1);
}

?>
never reached here
--EXPECTF--
111
Fatal error: Maximum execution time of 3 seconds exceeded in %s on line %d
