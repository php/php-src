--TEST--
Streams Based IPv6 TCP Loopback test
--SKIPIF--
<?php
	/* If IPv6 is supported on the platform this will error out with code 111 - Connection refused.
	   If IPv6 is NOT supported, $errno will be set to something else (indicating parse/getaddrinfo error)
	   Note: Might be a good idea to export an IPv6 support indicator (such as AF_INET6 exported by ext/sockets) */
	@stream_socket_client('tcp://[::1]:0', $errno);
	if ($errno != 111) die('skip IPv6 not supported.');
?>
--FILE--
<?php

  for ($i=0; $i<100; $i++) {
    $port = rand(10000, 65000);
    /* Setup socket server */
    $server = @stream_socket_server("tcp://[::1]:$port");
    if ($server) {
      break;
    }
  }

	if (!$server) {
		die('Unable to create AF_INET6 socket [server]');
	}

	/* Connect to it */
	$client = stream_socket_client("tcp://[::1]:$port");
	if (!$client) {
		die('Unable to create AF_INET6 socket [client]');
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
