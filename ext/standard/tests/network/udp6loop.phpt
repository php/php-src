--TEST--
Streams Based IPv6 UDP Loopback test
--SKIPIF--
<?php # vim:ft=php:
	/* If IPv6 is supported on the platform this will error out with code 111 -
	 * Connection refused.  If IPv6 is NOT supported, $errno will be set to
	 * something else (indicating parse/getaddrinfo error)
	 * Note: Might be a good idea to export an IPv6 support indicator
	 * (such as AF_INET6 exported by ext/sockets), however, since we
	 * cannot tell for sure if IPv6 works until we probe it at run time,
	 * this isn't really practical.
   	 */

	@stream_socket_client('tcp://[::1]:0', $errno);
	if ($errno != 111) die('skip IPv6 not supported.');
?>
--FILE--
<?php
	/* Setup socket server */
	$server = stream_socket_server('udp://[::1]:31337', $errno, $errstr, STREAM_SERVER_BIND);
	if (!$server) {
		die('Unable to create AF_INET6 socket [server]');
	}

	/* Connect to it */
	$client = stream_socket_client('udp://[::1]:31337');
	if (!$client) {
		die('Unable to create AF_INET6 socket [client]');
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
