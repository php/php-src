--TEST--
socket_import_stream: Test with multicasting
--EXTENSIONS--
sockets
--SKIPIF--
<?php

$s = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
$br = @socket_bind($s, '0.0.0.0', 58379);
if ($br === false)
    die("SKIP IPv4/port 58379 not available");
$so = @socket_set_option($s, IPPROTO_IP, MCAST_JOIN_GROUP, array(
    "group"	=> '224.0.0.23',
    "interface" => "lo",
));
if ($so === false)
    die("SKIP joining group 224.0.0.23 on interface lo failed");
?>
--FILE--
<?php

$stream = stream_socket_server("udp://0.0.0.0:58379", $errno, $errstr, STREAM_SERVER_BIND);
$sock = socket_import_stream($stream);
var_dump($sock);
$so = socket_set_option($sock, IPPROTO_IP, MCAST_JOIN_GROUP, array(
    "group"	=> '224.0.0.23',
    "interface" => "lo",
));
var_dump($so);

$sendsock = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
var_dump($sendsock);
$br = socket_bind($sendsock, '127.0.0.1');
$so = socket_sendto($sendsock, $m = "my message", strlen($m), 0, "224.0.0.23", 58379);
var_dump($so);

stream_set_blocking($stream, 0);
var_dump(fread($stream, strlen($m)));
echo "Done.\n";
?>
--EXPECTF--
object(Socket)#%d (0) {
}
bool(true)
object(Socket)#%d (0) {
}
int(10)
string(10) "my message"
Done.
