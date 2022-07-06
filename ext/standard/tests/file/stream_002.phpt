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

echo "Done\n";
?>
--EXPECTF--
Warning: stream_socket_client(): Unable to connect to  (Failed to parse address "") in %s on line %d
bool(false)
int(0)
string(26) "Failed to parse address """

Warning: stream_socket_client(): Unable to connect to [ (Failed to parse address "[") in %s on line %d
bool(false)
int(0)
string(27) "Failed to parse address "[""

Warning: stream_socket_client(): Unable to connect to [  (Failed to parse%saddress "[ ") in %s on line %d
bool(false)
int(0)
string(%d) "Failed to parse%saddress "[ ""

Warning: stream_socket_client(): Unable to connect to . (Failed to parse address ".") in %s on line %d
bool(false)
int(0)
string(27) "Failed to parse address ".""

Warning: stream_socket_client(): Unable to connect to 1 (Failed to parse address "1") in %s on line %d
bool(false)
int(0)
string(27) "Failed to parse address "1""
Done
