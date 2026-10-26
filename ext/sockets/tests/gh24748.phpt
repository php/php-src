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
var_dump(socket_get_option($socket, SOL_SOCKET, SO_LINGER));

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
?>
--EXPECT--
bool(true)
array(2) {
  ["l_onoff"]=>
  int(1)
  ["l_linger"]=>
  int(5)
}
socket_set_option(): Argument #4 ($value) must have key "usec"
bool(true)
bool(true)
array(2) {
  ["sec"]=>
  int(1)
  ["usec"]=>
  int(0)
}
