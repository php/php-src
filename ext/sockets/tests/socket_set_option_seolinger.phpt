--TEST--
Test if socket_set_option() works, option:SO_SEOLINGER
--DESCRIPTION--
-wrong params
-set/get params comparison
-l_linger not given
--EXTENSIONS--
sockets
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);

if (!$socket) {
        die('Unable to create AF_INET socket [socket]');
}
// wrong params
try {
    $retval_1 = socket_set_option( $socket, SOL_SOCKET, SO_LINGER, []);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// set/get comparison
$options = array("l_onoff" => 1, "l_linger" => 1);
$retval_2 = socket_set_option( $socket, SOL_SOCKET, SO_LINGER, $options);
$retval_3 = socket_get_option( $socket, SOL_SOCKET, SO_LINGER);

//l_linger not given
$options_2 = array("l_onoff" => 1);
try {
    var_dump(socket_set_option( $socket, SOL_SOCKET, SO_LINGER, $options_2));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump($retval_2);
var_dump($retval_3["l_linger"] === $options["l_linger"]);
// value of l_onoff is not always 1, Darwin returns 128
var_dump((bool)$retval_3["l_onoff"] === (bool)$options["l_onoff"]);

socket_close($socket);
?>
--EXPECT--
socket_set_option(): Argument #4 ($value) must have key "l_onoff"
socket_set_option(): Argument #4 ($value) must have key "l_linger"
bool(true)
bool(true)
bool(true)
