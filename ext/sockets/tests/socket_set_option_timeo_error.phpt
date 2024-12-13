--TEST--
socket_set_option() with SO_RCVTIMEO/SO_SNDTIMEO/SO_LINGER
--EXTENSIONS--
sockets
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if (!$socket) {
        die('Unable to create AF_INET socket [socket]');
}
$options_1 = array("sec" => 1, "usec" => "aaaaa");
$options_2 = array("sec" => new stdClass(), "usec" => "1");
$options_3 = array("l_onoff" => "aaaa", "l_linger" => "1");
$options_4 = array("l_onoff" => "1", "l_linger" => []);

try {
	socket_set_option( $socket, SOL_SOCKET, SO_RCVTIMEO, new stdClass);
} catch (\TypeError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	socket_set_option( $socket, SOL_SOCKET, SO_RCVTIMEO, $options_1);
} catch (\TypeError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	socket_set_option( $socket, SOL_SOCKET, SO_SNDTIMEO, $options_2);
} catch (\TypeError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	socket_set_option( $socket, SOL_SOCKET, SO_LINGER, "not good");
} catch (\TypeError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	socket_set_option( $socket, SOL_SOCKET, SO_LINGER, $options_3);
} catch (\TypeError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	socket_set_option( $socket, SOL_SOCKET, SO_LINGER, $options_4);
} catch (\TypeError $e) {
	echo $e->getMessage() . PHP_EOL;
}
?>
--EXPECT--
socket_set_option(): Argument #4 ($value) must be of type array, stdClass given
socket_set_option(): Argument #4 ($value) "usec" must be of type int, string given
socket_set_option(): Argument #4 ($value) "sec" must be of type int, stdClass given
socket_set_option(): Argument #4 ($value) must be of type array, string given
socket_set_option(): Argument #4 ($value) "l_onoff" must be of type int, string given
socket_set_option(): Argument #4 ($value) "l_linger" must be of type int, array given
