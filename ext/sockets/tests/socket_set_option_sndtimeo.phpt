--TEST--
Test if socket_set_option() works, option:SO_SNDTIMEO
--DESCRIPTION--
-wrong params
-set/get params comparison
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
        die('SKIP sockets extension not available.');
}
?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if (!$socket) {
        die('Unable to create AF_INET socket [socket]');
}
socket_set_block($socket);

//wrong params
try {
    $retval_1 = socket_set_option( $socket, SOL_SOCKET, SO_SNDTIMEO, []);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

//set/get comparison
$options = array("sec" => 1, "usec" => 0);
$retval_2 = socket_set_option( $socket, SOL_SOCKET, SO_SNDTIMEO, $options);
$retval_3 = socket_get_option( $socket, SOL_SOCKET, SO_SNDTIMEO);

var_dump($retval_2);
var_dump($retval_3 === $options);
socket_close($socket);
?>
--EXPECT--
socket_set_option(): Argument #4 ($value) must have key "sec"
bool(true)
bool(true)
