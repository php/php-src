--TEST--
Timeout within foreach loop
--FILE--
<?php

$t = 3;
set_time_limit($t);

foreach(range(0, 42) as $i) { 
	echo 1; 
	sleep(1);
}

?>
never reached here
--EXPECTF--
111
Fatal error: Maximum execution time of 3 seconds exceeded in %s on line %d
