--TEST--
Test sleep() function : basic functionality
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
echo "*** Testing sleep() : basic functionality ***\n";

function have_usleep() {
// if usleep is not avaiable on the native platform,
// usleep() calls will just do nothing
// so to check if we have usleep at runtime, we need to sleep for a short time
    $t = microtime(true);
    usleep(10000);
    return microtime(true) - $t >= 0.009; // 0.009 is 9ms, we asked usleep to sleep for 10ms
}


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
    echo "TEST FAILED - time is {$time} secs and sleep was {$sleeptime} secs\n";
}
if (!have_usleep()) {
    // ¯\_(ツ)_/¯
    echo "FRACTIONAL SLEEP TEST PASSED\n";
} else {
    $time = microtime(true);
    sleep(0.1);
    $time = microtime(true) - $time;
    if($time >= 0.09) {
        echo "FRACTIONAL SLEEP TEST PASSED\n";
    } else {
        echo "FRACTIONAL SLEEP TEST FAILED\n";
    }
}
?>
--EXPECTF--
*** Testing sleep() : basic functionality ***
Thread slept for %f seconds
Return value: 0
TEST PASSED
FRACTIONAL SLEEP TEST PASSED
