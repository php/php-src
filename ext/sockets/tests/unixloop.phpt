--TEST--
Unix domain socket Loopback test
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
	die('skip.. Not valid for Windows');
}
	if (!extension_loaded('sockets')) {
		die('skip sockets extension not available.');
	}
?>
--FILE--
<?php
	$sock_path = sprintf("/tmp/%s.sock", uniqid());

	if (file_exists($sock_path))
		die('Temporary socket already exists.');

	/* Setup socket server */
	$server = socket_create(AF_UNIX, SOCK_STREAM, 0);
	if (!$server) {
		die('Unable to create AF_UNIX socket [server]');
	}
	if (!socket_bind($server,  $sock_path)) {
		die("Unable to bind to $sock_path");
	}
	if (!socket_listen($server, 2)) {
		die('Unable to listen on socket');
	}
	
	/* Connect to it */
	$client = socket_create(AF_UNIX, SOCK_STREAM, 0);
	if (!$client) {
		die('Unable to create AF_UNIX socket [client]');
	}
	if (!socket_connect($client, $sock_path)) {
		die('Unable to connect to server socket');
	}

	/* Accept that connection */
	$socket = socket_accept($server);
	if (!$socket) {
		die('Unable to accept connection');
	}

	socket_write($client, "ABCdef123\n");

	$data = socket_read($socket, 10, PHP_BINARY_READ);
	var_dump($data);

	socket_close($client);
	socket_close($socket);
	socket_close($server);
	@unlink($sock_path);
?>
--EXPECT--
string(10) "ABCdef123
"
