--TEST--
Timeout within while loop
--FILE--
<?php

$t = 3;
set_time_limit($t);

while(1) { 
	echo 1; 
	sleep(1);
}

?>
never reached here
--EXPECTF--
111
Fatal error: Maximum execution time of 3 seconds exceeded in %s on line %d
