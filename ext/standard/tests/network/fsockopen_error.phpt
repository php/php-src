--TEST--
Test fsockopen() function : error conditions
--FILE--
<?php
echo "*** Testing fsockopen() : basic error conditions ***\n";

echo "\n-- Attempting to connect to a non-existent socket --\n";
$hostname = 'tcp://127.0.0.1'; // loopback address
$port = 31337;
$errno = null;
$errstr = null;
$timeout = 1.5;
var_dump( fsockopen($hostname, $port, $errno, $errstr, $timeout) );
var_dump($errstr);

echo "\n-- Attempting to connect using an invalid protocol --\n";
$hostname = 'invalid://127.0.0.1'; // loopback address
$port = 31337;
$errno = null;
$errstr = null;
$timeout = 1.5;
var_dump( fsockopen($hostname, $port, $errno, $errstr, $timeout) );
var_dump($errstr);

echo "Done";
?>
--EXPECTF--
*** Testing fsockopen() : basic error conditions ***

-- Attempting to connect to a non-existent socket --

Warning: fsockopen(): Unable to connect to tcp://127.0.0.1:31337 (%a) in %s on line %d
bool(false)
string(%d) "%a"

-- Attempting to connect using an invalid protocol --

Warning: fsockopen(): Unable to connect to invalid://127.0.0.1:31337 (Unable to find the socket transport "invalid" - did you forget to enable it when you configured PHP?) in %s on line %d
bool(false)
string(100) "Unable to find the socket transport "invalid" - did you forget to enable it when you configured PHP?"
Done
