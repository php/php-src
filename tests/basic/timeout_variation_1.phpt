--TEST--
Timeout within function
--FILE--
<?php

$t = 3;
set_time_limit($t);

function hello ($t) { 
	echo "call"; 
	sleep($t*2);
}

hello($t);


?>
never reached here
--EXPECTF--
call
Fatal error: Maximum execution time of 3 seconds exceeded in %s on line %d
