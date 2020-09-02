--TEST--
Test usleep() function
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
set_time_limit(20);

echo "*** Testing usleep() : basic functionality ***\n";

$sleeptime = 1000000; // == 1 seconds
// Test passes if sleeps for at least 98% of specified time
$sleeplow = $sleeptime - ($sleeptime * 2 /100);

$time_start = microtime(true);

// Sleep for a while
usleep($sleeptime);

$time_end = microtime(true);
$time = ($time_end - $time_start) * 1000 * 1000;

$summary = "Thread slept for " . $time . " micro-seconds\n";

if ($time >= $sleeplow) {
    echo "TEST PASSED: $summary";
} else {
    echo "TEST FAILED: $summary";
}
?>
--EXPECTF--
*** Testing usleep() : basic functionality ***
TEST PASSED: Thread slept for %f micro-seconds
