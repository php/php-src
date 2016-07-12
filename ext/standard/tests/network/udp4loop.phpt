--TEST--
Streams Based IPv4 UDP Loopback test
--FILE--
<?php
	/* Setup socket server */
	for ($port = 31338; $port < 31500; ++$port) {
	  $uri = "udp://127.0.0.1:$port";
	  $server = @stream_socket_server($uri, $errno, $errstr, STREAM_SERVER_BIND);
	  if ($server) break;
	}
	if (!$server) {
		die('Unable to create AF_INET socket [server]: ' . $errstr);
	}

	/* Connect to it */
	$client = stream_socket_client($uri);
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
