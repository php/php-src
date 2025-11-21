--TEST--
time_sleep_until() function - basic test for time_sleep_until()
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (!function_exists("time_sleep_until")) die('skip time_sleep_until() not available');
?>
--CREDITS--
Manuel Baldassarri mb@ideato.it
Michele Orselli mo@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--FILE--
<?php
/**
 * Checks if a value is within a specified tolerance of a target value.
 *
 * @param float $val       The value to test.
 * @param float $target    The target value.
 * @param float $tolerance The allowed tolerance.
 *
 * @return bool True if the absolute difference is less than or equal to the tolerance, false otherwise.
 */
function isWithinTolerance(float $val, float $target, float $tolerance): bool
{
    return abs($val - $target) <= $tolerance;
}

$startTime = microtime(true);
// Calculate the target sleep time (2 seconds from now)
$targetTime = $startTime + 2;
// Sleep until the target time
$dump = time_sleep_until($targetTime);
// Capture the current time immediately after sleep
$timeAfterSleep = microtime(true);
var_dump($dump);
if (stripos(PHP_OS, 'WIN') === 0) {
    // on windows, time_sleep_until has millisecond accuracy while microtime() is accurate
    // to 10th of a second. this means there can be up to a .9 millisecond difference
    // which will fail this test. this test randomly fails on Windows and this is the cause.
    //
    // fix: round to nearest millisecond
    // passes for up to .5 milliseconds less, fails for more than .5 milliseconds
    // should be fine since time_sleep_until() on Windows is accurate to the
    // millisecond(.5 rounded up is 1 millisecond)
    $tolerance = 0.05;
} elseif (stripos(PHP_OS, 'DARWIN') === 0) {
    // macOS: time_sleep_until() may wake up slightly early for unknown reasons.
    // MacOS Github CI late by 106ms has been observed.
    $tolerance = 0.2;
} elseif(PHP_OS === "FreeBSD"){
    // FreeBSD Github CI late by 74ms has been observed...
    $tolerance = 0.2;
}else {
    // Default tolerance
    $tolerance = 0.01;
}

if (1 && isWithinTolerance($timeAfterSleep, $targetTime, $tolerance)) {
    echo "Success" . PHP_EOL;
} else {
    echo "Failure" . PHP_EOL;
    var_dump([
        "startTime" => $startTime,
        "targetTime" => $targetTime,
        "timeAfterSleep" => $timeAfterSleep,
        "diff" => $timeAfterSleep - $targetTime,
        "tolerance" => $tolerance,
        "distanceFromTarget" => abs($timeAfterSleep - $targetTime),
    ]);
}
?>
--EXPECT--
bool(true)
Success
