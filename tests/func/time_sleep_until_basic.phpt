--TEST--
time_sleep_until —  Make the script sleep until the specified time 
--CREDITS--
Àlex Corretgé - alex@corretge.cat
--FILE--
<?php

$timeA = time();
time_sleep_until($timeA+3);
$timeB = time();
echo ($timeB - $timeA) . " seconds.";

?>
--EXPECT--
3 seconds.