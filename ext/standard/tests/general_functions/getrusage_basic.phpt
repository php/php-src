--TEST--
Test getrusage() function: basic test
--SKIPIF--
<?php if (!function_exists("getrusage")) print "skip"; ?>
--FILE--
<?php
echo "Simple testcase for getrusage() function\n";

$dat = getrusage();

if (!is_array($dat)) {
    echo "TEST FAILED : getrusage should return an array\n";
}

// echo the fields which are common to all platforms
echo "User time used (seconds) " . $dat["ru_utime.tv_sec"] . "\n";
echo "User time used (microseconds) " . $dat["ru_utime.tv_usec"] . "\n";
?>
--EXPECTF--
Simple testcase for getrusage() function
User time used (seconds) %d
User time used (microseconds) %d
