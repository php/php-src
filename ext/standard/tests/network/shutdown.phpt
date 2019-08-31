--TEST--
stream_socket_shutdown() test on IPv4 TCP Loopback
--SKIPIF--
<?php
	function_exists('stream_socket_shutdown') or die('skip stream_socket_shutdown() is not supported.');
?>
--FILE--
<?php

  for ($i=0; $i<100; $i++) {
    $port = rand(10000, 65000);
    /* Setup socket server */
    $server = @stream_socket_server("tcp://127.0.0.1:$port");
    if ($server) {
      break;
    }
  }

if (!$server) {
		die('Unable to create AF_INET socket [server]');
	}

	/* Connect and send request 1 */
	$client1 = stream_socket_client("tcp://127.0.0.1:$port");
	if (!$client1) {
		die('Unable to create AF_INET socket [client]');
	}
	@fwrite($client1, "Client 1\n");
	stream_socket_shutdown($client1, STREAM_SHUT_WR);
	@fwrite($client1, "Error 1\n");

	/* Connect and send request 2 */
	$client2 = stream_socket_client("tcp://127.0.0.1:$port");
	if (!$client2) {
		die('Unable to create AF_INET socket [client]');
	}
	@fwrite($client2, "Client 2\n");
	stream_socket_shutdown($client2, STREAM_SHUT_WR);
	@fwrite($client2, "Error 2\n");

	/* Accept connection 1 */
	$socket = stream_socket_accept($server);
	if (!$socket) {
		die('Unable to accept connection');
	}
	@fwrite($socket, fgets($socket));
	@fwrite($socket, fgets($socket));
	fclose($socket);

	/* Read Response 1 */
	echo fgets($client1);
	echo fgets($client1);

	/* Accept connection 2 */
	$socket = stream_socket_accept($server);
	if (!$socket) {
		die('Unable to accept connection');
	}
	@fwrite($socket, fgets($socket));
	@fwrite($socket, fgets($socket));
	fclose($socket);

	/* Read Response 2 */
	echo fgets($client2);
	echo fgets($client2);

	fclose($client1);
	fclose($client2);
	fclose($server);
?>
--EXPECT--
Client 1
Client 2
