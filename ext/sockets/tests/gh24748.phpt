--TEST--
GH-24748 (socket_set_option() crashes when the value is an object with declared properties)
--EXTENSIONS--
sockets
--FILE--
<?php
class Linger {
    public $l_onoff = 1;
    public $l_linger = 5;
}

class Timeout {
    public $sec = 1;
    public int $usec;
}

$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
var_dump(socket_set_option($socket, SOL_SOCKET, SO_LINGER, new Linger()));
var_dump(socket_get_option($socket, SOL_SOCKET, SO_LINGER)["l_linger"]);

try {
    socket_set_option($socket, SOL_SOCKET, SO_RCVTIMEO, new Timeout());
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

$timeout = new Timeout();
$timeout->usec = 0;
var_dump(socket_set_option($socket, SOL_SOCKET, SO_RCVTIMEO, $timeout));
var_dump(socket_set_option($socket, SOL_SOCKET, SO_SNDTIMEO, $timeout));
var_dump(socket_get_option($socket, SOL_SOCKET, SO_SNDTIMEO));

$linger = new stdClass();
$linger->l_onoff = new stdClass();
$linger->l_linger = 5;
set_error_handler(function () use ($linger) {
    $tmp = (array) $linger;
    $linger->extra = 1;
    unset($tmp);
    return true;
});
var_dump(socket_set_option($socket, SOL_SOCKET, SO_LINGER, $linger));
restore_error_handler();

try {
    socket_set_option($socket, SOL_SOCKET, SO_LINGER, new ArrayObject(['l_onoff' => 1, 'l_linger' => 70000]));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
int(5)
socket_set_option(): Argument #4 ($value) must have key "usec"
bool(true)
bool(true)
array(2) {
  ["sec"]=>
  int(1)
  ["usec"]=>
  int(0)
}
bool(true)
socket_set_option(): Argument #4 ($value) "l_linger" must be between 0 and 65535
