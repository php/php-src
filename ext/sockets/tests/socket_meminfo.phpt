--TEST--
socket_get_option MEMINFO;
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (!defined("SO_MEMINFO")) die('skip SO_MEMINFO test');
?>
--FILE--
<?php
$server = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if (!$server) {
    die('Unable to create AF_INET socket [server]');
}

if (!socket_bind($server, '127.0.0.1', 0)) {
    die("Unable to bind to 127.0.0.1");
}

if (!socket_listen($server, 2)) {
    die('Unable to listen on socket');
}

socket_getsockname($server, $unused, $port);

/* Connect to it */
$client = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if (!$client) {
    die('Unable to create AF_INET socket [client]');
}
if (!socket_connect($client, '127.0.0.1', $port)) {
    die('Unable to connect to server socket');
}

/* Accept that connection */
$socket = socket_accept($server);
if (!$socket) {
    die('Unable to accept connection');
}

socket_write($client, "SO_MEMINFO\n");

socket_read($socket, strlen("SO_MEMINFO"), PHP_BINARY_READ);
$data = socket_get_option($socket, SOL_SOCKET, SO_MEMINFO);
var_dump($data);

socket_close($client);
socket_close($socket);
socket_close($server);
?>
--EXPECTF--
array(9) {
  ["rmem_alloc"]=>
  int(%d)
  ["rcvbuf"]=>
  int(%d)
  ["wmem_alloc"]=>
  int(%d)
  ["sndbuf"]=>
  int(%d)
  ["fwd_alloc"]=>
  int(%d)
  ["wmem_queued"]=>
  int(%d)
  ["optmem"]=>
  int(%d)
  ["backlog"]=>
  int(%d)
  ["drops"]=>
  int(%d)
}
