--TEST--
Timeout within array_walk
--FILE--
<?php

$t = 3;
set_time_limit($t);

function cb(&$i, $k, $p)
{ 
	echo 1; 
	sleep(1);
}

$a = array(1 => 1, 2 => 1, 3 => 1, 4 => 1, 5 => 1, 6 => 1, 7 => 1);
array_walk($a, "cb", "junk");

?>
never reached here
--EXPECTF--
111
Fatal error: Maximum execution time of 3 seconds exceeded in %s on line %d
