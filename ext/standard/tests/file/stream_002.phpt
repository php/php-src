--TEST--
stream_socket_client() and invalid arguments
--FILE--
<?php

$a = NULL;
$b = NULL;
var_dump(stream_socket_client("", $a, $b));
var_dump($a, $b);
var_dump(stream_socket_client("[", $a, $b));
var_dump($a, $b);
var_dump(stream_socket_client("[ ", $a, $b));
var_dump($a, $b);
var_dump(stream_socket_client(".", $a, $b));
var_dump($a, $b);
var_dump(stream_socket_client(1, $a, $b));
var_dump($a, $b);
var_dump(stream_socket_client(array(), $a, $b));
var_dump($a, $b);

echo "Done\n";
?>
--EXPECTF--	
Warning: stream_socket_client(): unable to connect to  (Failed to parse address "") in %s on line %d
bool(false)
int(0)
string(26) "Failed to parse address """

Warning: stream_socket_client(): unable to connect to [ (Failed to parse address "[") in %s on line %d
bool(false)
int(0)
string(27) "Failed to parse address "[""

Warning: stream_socket_client(): unable to connect to [  (Failed to parse IPv6 address "[ ") in %s on line %d
bool(false)
int(0)
string(33) "Failed to parse IPv6 address "[ ""

Warning: stream_socket_client(): unable to connect to . (Failed to parse address ".") in %s on line %d
bool(false)
int(0)
string(27) "Failed to parse address ".""

Warning: stream_socket_client(): unable to connect to 1 (Failed to parse address "1") in %s on line %d
bool(false)
int(0)
string(27) "Failed to parse address "1""

Warning: stream_socket_client() expects parameter 1 to be string, array given in %s on line %d
bool(false)
int(0)
string(27) "Failed to parse address "1""
Done
--UEXPECTF--
Warning: stream_socket_client(): unable to connect to  (Failed to parse address "") in %s on line %d
bool(false)
int(0)
unicode(26) "Failed to parse address """

Warning: stream_socket_client(): unable to connect to [ (Failed to parse address "[") in %s on line %d
bool(false)
int(0)
unicode(27) "Failed to parse address "[""

Warning: stream_socket_client(): unable to connect to [  (Failed to parse IPv6 address "[ ") in %s on line %d
bool(false)
int(0)
unicode(33) "Failed to parse IPv6 address "[ ""

Warning: stream_socket_client(): unable to connect to . (Failed to parse address ".") in %s on line %d
bool(false)
int(0)
unicode(27) "Failed to parse address ".""

Warning: stream_socket_client(): unable to connect to 1 (Failed to parse address "1") in %s on line %d
bool(false)
int(0)
unicode(27) "Failed to parse address "1""

Warning: stream_socket_client() expects parameter 1 to be binary string, array given in %s on line %d
bool(false)
int(0)
unicode(27) "Failed to parse address "1""
Done
