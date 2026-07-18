--TEST--
GH-22617: Persistent abstract unix domain sockets resolved to wrong resource
--CREDITS--
Roysten
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== "Linux") die("skip abstract unix domain sockets are Linux-only");
if (!function_exists("pfsockopen")) die("skip pfsockopen() not available");
?>
--FILE--
<?php
// Abstract sockets: the address is prefixed with a null byte.
$name1 = "gh22617_" . getmypid() . "_1";
$name2 = "gh22617_" . getmypid() . "_2";

$server1 = stream_socket_server("unix://\0$name1", $errno, $errstr);
$server2 = stream_socket_server("unix://\0$name2", $errno, $errstr);
var_dump($server1 !== false, $server2 !== false);

// Connect to the first abstract socket.
$socket1 = pfsockopen("unix://\0$name1", 0, $errno1, $errstr1);
var_dump($socket1 !== false);
var_dump(get_resource_type($socket1));

// Connect to the second abstract socket.
$socket2 = pfsockopen("unix://\0$name2", 0, $errno2, $errstr2);
var_dump($socket2 !== false);
var_dump(get_resource_type($socket2));

// The two distinct abstract sockets must resolve to distinct resources.
var_dump((int) $socket1 !== (int) $socket2);

fclose($socket1);
fclose($socket2);
fclose($server1);
fclose($server2);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
string(17) "persistent stream"
bool(true)
string(17) "persistent stream"
bool(true)
