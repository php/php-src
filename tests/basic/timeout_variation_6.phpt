--TEST--
Timeout within function trowing exception before timeout reached
--SKIPIF--
<?php 
	if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "timeout_config.inc";

set_time_limit($t);

function f($t) { 
	echo "call";
	busy_wait($t-1);
	throw new Exception("exception before timeout");
}

f($t);
?>
never reached here
--EXPECTF--
call
Fatal error: Uncaught Exception: exception before timeout in %s:%d
Stack trace:
#0 %s(%d): f(%d)
#1 {main}
  thrown in %s on line %d
