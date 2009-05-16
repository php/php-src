--TEST--
time_nanosleep — Delay for a number of seconds and nanoseconds
--SKIPIF--
<?php if (!function_exists('time_nanosleep')) die("skip"); ?>
--CREDITS--
Àlex Corretgé - alex@corretge.cat
--FILE--
<?php
$nano = time_nanosleep(2, 100000);

if ($nano === true) {
    echo "Slept for 2 seconds, 100 milliseconds.\n";
} elseif ($nano === false) {
    echo "Sleeping failed.\n";
} elseif (is_array($nano)) {
    $seconds = $nano['seconds'];
    $nanoseconds = $nano['nanoseconds'];
    echo "Interrupted by a signal.\n";
    echo "Time remaining: $seconds seconds, $nanoseconds nanoseconds.";
}
?>
--EXPECT--
Slept for 2 seconds, 100 milliseconds.
