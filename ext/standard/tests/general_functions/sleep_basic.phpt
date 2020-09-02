--TEST--
Test sleep() function : basic functionality
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
echo "*** Testing sleep() : basic functionality ***\n";

$sleeptime = 1; // sleep for 1 seconds

set_time_limit(20);

$time_start = microtime(true);

// Sleep for a while
$result = sleep($sleeptime);

// Test passes if sleeps for at least 98% of specified time
$sleeplow = $sleeptime - ($sleeptime * 2 /100);

$time_end = microtime(true);
$time = $time_end - $time_start;

echo "Thread slept for " . $time . " seconds\n";
echo "Return value: " . $result . "\n";

if ($time >= $sleeplow) {
    echo "TEST PASSED\n";
} else {
    echo "TEST FAILED - time is ${time} secs and sleep was ${sleeptime} secs\n";
}
?>
--EXPECTF--
*** Testing sleep() : basic functionality ***
Thread slept for %f seconds
Return value: 0
TEST PASSED
