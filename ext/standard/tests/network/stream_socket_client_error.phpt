--TEST--
Test stream_socket_client() function : error conditions
--FILE--
<?php
echo "*** Testing stream_socket_client() : basic error conditions ***\n";

echo "\n-- Attempting to connect using an invalid protocol --\n";
$address = 'invalid://127.0.0.1:31337';
$errno = null;
$errstr = null;
$timeout = 1.5;
var_dump( stream_socket_client($address, $errno, $errstr, $timeout) );
var_dump($errno);
var_dump($errstr);

echo "Done";
?>
--EXPECTF--
*** Testing stream_socket_client() : basic error conditions ***

-- Attempting to connect using an invalid protocol --

Warning: stream_socket_client(): Unable to connect to invalid://127.0.0.1:31337 (Unable to find the socket transport "invalid" - did you forget to enable it when you configured PHP?) in %s on line %d
bool(false)
int(0)
string(100) "Unable to find the socket transport "invalid" - did you forget to enable it when you configured PHP?"
Done
