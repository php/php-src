--TEST--
Test usleep() function
--FILE--
<?php
/* Prototype  : void usleep  ( int $micro_seconds  )
 * Description: Delays program execution for the given number of micro seconds. 
 * Source code: ext/standard/basic_functions.c
 */
 
echo "*** Testing usleep() : basic functionality ***\n";

set_time_limit(20); 

$sleeptime = 5000000; // == 5 seconds
// Test passes if sleeps for at least 98% of specified time 
$sleeplow = $sleeptime - ($sleeptime * 2 /100);

$time_start = microtime(true);

// Sleep for a while
usleep($sleeptime);

$time_end = microtime(true);
$time = ($time_end - $time_start) * 1000 * 1000;

echo "Thread slept for " . $time . " micro-seconds\n";

if ($time >= $sleeplow) {
	echo "TEST PASSED\n";
} else {
	echo "TEST FAILED - time is ${time} micro-seconds and sleep was ${sleeptime} micro-seconds\n";
}
?>
===DONE===
--EXPECTF--
*** Testing usleep() : basic functionality ***
Thread slept for %f micro-seconds
TEST PASSED
===DONE===
