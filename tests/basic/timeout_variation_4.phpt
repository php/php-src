--TEST--
Timeout within call_user_func
--SKIPIF--
<?php 
	if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "timeout_config.inc";

set_time_limit($t);

function hello ($t) { 
	echo "call", PHP_EOL; 
	busy_wait($t*2);
}

call_user_func('hello', $t);

?>
never reached here
--EXPECTF--
call

Fatal error: Maximum execution time of 3 seconds exceeded in %s on line %d
