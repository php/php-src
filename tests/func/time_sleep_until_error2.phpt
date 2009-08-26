--TEST--
time_sleep_until —  Make the script sleep until the specified time 
--CREDITS--
Àlex Corretgé - alex@corretge.cat
--FILE--
<?php

$timeA = time();
time_sleep_until($timeA-3);
$timeB = time();
echo ($timeB - $timeA) . " seconds.";

?>
--EXPECTF--
Warning: time_sleep_until(): Sleep until to time is less than current time in %s.php on line %d
0 seconds.