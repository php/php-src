--TEST--
Streams Based IPv4 UDP Loopback test
--FILE--
<?php
	/* Setup socket server */
	$server = stream_socket_server('udp://127.0.0.1:31338', $errno, $errstr, STREAM_SERVER_BIND);
	if (!$server) {
		die('Unable to create AF_INET socket [server]');
	}

	/* Connect to it */
	$client = stream_socket_client('udp://127.0.0.1:31338');
	if (!$client) {
		die('Unable to create AF_INET socket [client]');
	}

	fwrite($client, "ABCdef123\n");

	$data = fread($server, 10);
	var_dump($data);

	fclose($client);
	fclose($server);
?>
--EXPECT--
string(10) "ABCdef123
"
