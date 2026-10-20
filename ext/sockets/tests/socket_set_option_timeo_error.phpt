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
$options_5 = array("l_onoff" => PHP_INT_MAX, "l_linger" => "1");
$options_6 = array("l_onoff" => "1", "l_linger" => PHP_INT_MAX);

try {
	socket_set_option( $socket, SOL_SOCKET, SO_RCVTIMEO, new stdClass);
} catch (\Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	socket_set_option( $socket, SOL_SOCKET, SO_RCVTIMEO, $options_1);
} catch (\Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	socket_set_option( $socket, SOL_SOCKET, SO_SNDTIMEO, $options_2);
} catch (\Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
	socket_set_option( $socket, SOL_SOCKET, SO_RCVTIMEO, "not good");
} catch (\Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
	socket_set_option( $socket, SOL_SOCKET, SO_LINGER, "not good neither");
} catch (\Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
	socket_set_option( $socket, SOL_SOCKET, SO_LINGER, $options_3);
} catch (\Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
	socket_set_option( $socket, SOL_SOCKET, SO_LINGER, $options_4);
} catch (\Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
	socket_set_option( $socket, SOL_SOCKET, SO_LINGER, $options_5);
} catch (\Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
	socket_set_option( $socket, SOL_SOCKET, SO_LINGER, $options_6);
} catch (\Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
ValueError: socket_set_option(): Argument #4 ($value) must have key "sec"

Warning: Object of class stdClass could not be converted to int in %s on line %d
TypeError: socket_set_option(): Argument #4 ($value) must be of type array when argument #3 ($option) is SO_RCVTIMEO, string given
TypeError: socket_set_option(): Argument #4 ($value) must be of type array when argument #3 ($option) is SO_LINGER, string given
ValueError: socket_set_option(): Argument #4 ($value) "l_onoff" must be between 0 and %d
ValueError: socket_set_option(): Argument #4 ($value) "l_linger" must be between 0 and %d
