--TEST--
socket_export_stream: Test with multicasting
--EXTENSIONS--
sockets
--SKIPIF--
<?php

$s = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
socket_bind($s, '0.0.0.0', 0);
$so = @socket_set_option($s, IPPROTO_IP, MCAST_JOIN_GROUP, array(
    "group"	=> '224.0.0.23',
    "interface" => "lo",
));
if ($so === false)
    die("SKIP joining group 224.0.0.23 on interface lo failed");
?>
--FILE--
<?php

$sock = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
socket_bind($sock, '0.0.0.0', 0);
socket_getsockname($sock, $unused, $port);
$stream = socket_export_stream($sock);
var_dump($stream);
$so = socket_set_option($sock, IPPROTO_IP, MCAST_JOIN_GROUP, array(
    "group"	=> '224.0.0.23',
    "interface" => "lo",
));
var_dump($so);

$sendsock = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
var_dump($sendsock);
$br = socket_bind($sendsock, '127.0.0.1');
$so = socket_sendto($sendsock, $m = "my message", strlen($m), 0, "224.0.0.23", $port);
var_dump($so);

stream_set_blocking($stream, 0);
var_dump(fread($stream, strlen($m)));
echo "Done.\n";
?>
--EXPECTF--
resource(%d) of type (stream)
bool(true)
object(Socket)#%d (0) {
}
int(10)
string(10) "my message"
Done.
