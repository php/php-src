--TEST--
socket_recvfrom() with IPv4 socket missing port argument
--EXTENSIONS--
sockets
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);

try {
	socket_recvfrom($socket, $buffering, 20, 0, $address);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ArgumentCountError: Wrong parameter count for socket_recvfrom()
