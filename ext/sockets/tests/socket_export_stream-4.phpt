--TEST--
socket_export_stream: effects of closing
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
	die('SKIP sockets extension not available.');
}
if(substr(PHP_OS, 0, 3) == 'WIN' ) {
  die("skip Not Valid for Windows");
}
--FILE--
<?php

function test($stream, $sock) {
	if ($stream !== null) {
		echo "stream_set_blocking ";
		print_r(stream_set_blocking($stream, 0));
		echo "\n";
	}
	if ($sock !== null) {
		echo "socket_set_block ";
		print_r(socket_set_block($sock));
		echo "\n";
		echo "socket_get_option ";
		print_r(socket_get_option($sock, SOL_SOCKET, SO_TYPE));
		echo "\n";
	}
	echo "\n";
}

echo "normal\n";
$sock0 = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
socket_bind($sock0, '0.0.0.0', 58380);
$stream0 = socket_export_stream($sock0);
test($stream0, $sock0);

echo "\nunset stream\n";
$sock1 = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
socket_bind($sock1, '0.0.0.0', 58381);
$stream1 = socket_export_stream($sock1);
unset($stream1);
test(null, $sock1);

echo "\nunset socket\n";
$sock2 = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
socket_bind($sock2, '0.0.0.0', 58382);
$stream2 = socket_export_stream($sock2);
unset($sock2);
test($stream2, null);

echo "\nclose stream\n";
$sock3 = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
socket_bind($sock3, '0.0.0.0', 58383);
$stream3 = socket_export_stream($sock3);
fclose($stream3);
test($stream3, $sock3);

echo "\nclose socket\n";
$sock4 = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
socket_bind($sock4, '0.0.0.0', 58484);
$stream4 = socket_export_stream($sock4);
socket_close($sock4);
test($stream4, $sock4);

echo "Done.\n";
--EXPECTF--
normal
stream_set_blocking 1
socket_set_block 1
socket_get_option 2


unset stream
socket_set_block 1
socket_get_option 2


unset socket
stream_set_blocking 1


close stream
stream_set_blocking 
Warning: stream_set_blocking(): supplied resource is not a valid stream resource in %s on line %d

socket_set_block 
Warning: socket_set_block(): unable to set blocking mode [%d]: %s in %s on line %d

socket_get_option 
Warning: socket_get_option(): unable to retrieve socket option [%d]: %s in %s on line %d



close socket
stream_set_blocking 
Warning: stream_set_blocking(): supplied resource is not a valid stream resource in %s on line %d

socket_set_block 
Warning: socket_set_block(): supplied resource is not a valid Socket resource in %s on line %d

socket_get_option 
Warning: socket_get_option(): supplied resource is not a valid Socket resource in %s on line %d


Done.
