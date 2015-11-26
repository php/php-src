--TEST--
Test fsockopen() function : error conditions 
--FILE--
<?php
/* Prototype  : proto resource fsockopen(string hostname, int port [, int errno [, string errstr [, float timeout]]])
 * Description: Open Internet or Unix domain socket connection 
 * Source code: ext/standard/fsock.c
 * Alias to functions: 
 */


echo "*** Testing fsockopen() : basic error conditions ***\n";


echo "\n-- Testing fsockopen() function with more than expected no. of arguments --\n";
$hostname = 'string_val';
$port = 10;
$errno = 10;
$errstr = 'string_val';
$timeout = 10.5;
$extra_arg = 10;
var_dump( fsockopen($hostname, $port, $errno, $errstr, $timeout, $extra_arg) );
var_dump($errstr);
var_dump($errno);

echo "\n-- Testing fsockopen() function with less than expected no. of arguments --\n";
var_dump( fsockopen() );

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

-- Testing fsockopen() function with more than expected no. of arguments --

Warning: fsockopen() expects at most 5 parameters, 6 given in %s on line %d
bool(false)
string(10) "string_val"
int(10)

-- Testing fsockopen() function with less than expected no. of arguments --

Warning: fsockopen() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

-- Attempting to connect to a non-existent socket --

Warning: fsockopen(): unable to connect to tcp://127.0.0.1:31337 (%a) in %s on line %d
bool(false)
string(%d) "%a"

-- Attempting to connect using an invalid protocol --

Warning: fsockopen(): unable to connect to invalid://127.0.0.1:31337 (Unable to find the socket transport "invalid" - did you forget to enable it when you configured PHP?) in %s on line %d
bool(false)
string(100) "Unable to find the socket transport "invalid" - did you forget to enable it when you configured PHP?"
Done
