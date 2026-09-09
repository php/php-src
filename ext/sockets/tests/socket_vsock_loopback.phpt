--TEST--
AF_VSOCK stream round trip over the loopback transport
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (!defined('AF_VSOCK')) {
    die('skip AF_VSOCK not available');
}
if (!defined('VMADDR_CID_LOCAL')) {
    die('skip no vsock loopback context ID');
}
$socket = @socket_create(AF_VSOCK, SOCK_STREAM, 0);
if ($socket === false) {
    die('skip no vsock transport available');
}
if (!@socket_bind($socket, (string) VMADDR_CID_LOCAL, VMADDR_PORT_ANY)) {
    die('skip unable to bind a vsock port');
}
?>
--FILE--
<?php
$server = socket_create(AF_VSOCK, SOCK_STREAM, 0);
var_dump(socket_bind($server, (string) VMADDR_CID_LOCAL, VMADDR_PORT_ANY));
var_dump(socket_listen($server));

$cid = $port = null;
var_dump(socket_getsockname($server, $cid, $port));
var_dump($cid);
var_dump($port !== 0 && $port !== VMADDR_PORT_ANY);

$client = socket_create(AF_VSOCK, SOCK_STREAM, 0);
var_dump(socket_connect($client, (string) VMADDR_CID_LOCAL, $port));

$conn = socket_accept($server);
var_dump($conn instanceof Socket);

var_dump(socket_write($client, 'ping'));
var_dump(socket_read($conn, 4));

var_dump(socket_write($client, 'pong'));
$data = $from = $from_port = null;
var_dump(socket_recvfrom($conn, $data, 16, 0, $from, $from_port));
var_dump($data);

var_dump(socket_write($client, 'peng'));
$data = $from = null;
var_dump(socket_recvfrom($conn, $data, 16, 0, $from));
var_dump($data);

$peer_cid = $peer_port = null;
var_dump(socket_getpeername($client, $peer_cid, $peer_port));
var_dump($peer_cid);
var_dump($peer_port === $port);

$wildcard = socket_create(AF_VSOCK, SOCK_STREAM, 0);
var_dump(socket_bind($wildcard, (string) VMADDR_CID_ANY, VMADDR_PORT_ANY));
socket_close($wildcard);

$signed = socket_create(AF_VSOCK, SOCK_STREAM, 0);
var_dump(socket_bind($signed, '-1', VMADDR_PORT_ANY));
socket_close($signed);
socket_close($conn);
socket_close($client);
socket_close($server);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
string(1) "1"
bool(true)
bool(true)
bool(true)
int(4)
string(4) "ping"
int(4)
int(4)
string(4) "pong"
int(4)
int(4)
string(4) "peng"
bool(true)
string(1) "1"
bool(true)
bool(true)
bool(true)
