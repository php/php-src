--TEST--
time_sleep_until() : Make the script sleep until the specified time
--SKIPIF--
<?php
	function_exists('time_sleep_until') or die('skip time_sleep_until() is not supported in this build.');
?>
--CREDITS--
Àlex Corretgé - alex@corretge.cat
--FILE--
<?php

$timeA = time();
time_sleep_until($timeA+3);
$timeB = time();
echo ($timeB - $timeA) . " seconds.\n";

?>
===DONE===
--EXPECT--
3 seconds.
===DONE===
	