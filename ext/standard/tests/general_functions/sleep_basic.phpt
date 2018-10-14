--TEST--
Test sleep() function : basic functionality
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
/* Prototype  : int sleep  ( int $seconds  )
 * Description: Delays the program execution for the given number of seconds .
 * Source code: ext/standard/basic_functions.c
 */

echo "*** Testing sleep() : basic functionality ***\n";

$sleeptime = 5; // sleep for 5 seconds

set_time_limit(20);

$time_start = microtime(true);

// Sleep for a while
sleep($sleeptime);

// Test passes if sleeps for at least 98% of specified time
$sleeplow = $sleeptime - ($sleeptime * 2 /100);

$time_end = microtime(true);
$time = $time_end - $time_start;

echo "Thread slept for " . $time . " seconds\n";

if ($time >= $sleeplow) {
	echo "TEST PASSED\n";
} else {
	echo "TEST FAILED - time is ${time} secs and sleep was ${sleeptime} secs\n";
}
?>
===DONE===
--EXPECTF--
*** Testing sleep() : basic functionality ***
Thread slept for %f seconds
TEST PASSED
===DONE===
