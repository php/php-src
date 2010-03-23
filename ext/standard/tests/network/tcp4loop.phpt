--TEST--
Streams Based IPv4 TCP Loopback test
--FILE--
<?php # vim:ft=php:
	/* Setup socket server */
	$server = stream_socket_server('tcp://127.0.0.1:31337');
	if (!$server) {
		die('Unable to create AF_INET socket [server]');
	}

	/* Connect to it */
	$client = stream_socket_client('tcp://127.0.0.1:31337');
	if (!$client) {
		die('Unable to create AF_INET socket [client]');
	}

	/* Accept that connection */
	$socket = stream_socket_accept($server);
	if (!$socket) {
		die('Unable to accept connection');
	}

	fwrite($client, "ABCdef123\n");

	$data = fread($socket, 10);
	var_dump($data);

	fclose($client);
	fclose($socket);
	fclose($server);
?>
--EXPECT--
string(10) "ABCdef123
"
