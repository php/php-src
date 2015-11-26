--TEST--
testing fsockopen() with udp sockets
--FILE--
<?php

$hostname = 'udp://127.0.0.1';
$port = '31337';

echo "Open a server socket\n";
$server = stream_socket_server($hostname . ':' . $port, $errno, $errstr, STREAM_SERVER_BIND);

echo "\nCalling fsockopen():\n";
$client = fsockopen($hostname, $port);
var_dump($client);

echo "\nPass some data between the sockets:\n";
fwrite($client, "0123456789");
var_dump(fread($server, 10));
fclose($client);

echo "\nCalling fsockopen() with address and port in same string:\n";
$address = $hostname . ':' . $port;
$second_client = fsockopen($address);
var_dump($second_client);

echo "\nPass some data between the sockets:\n";
fwrite($second_client, "0123456789");
var_dump(fread($server, 10));
fclose($second_client);

echo "Done";

?>
--EXPECTF--
Open a server socket

Calling fsockopen():
resource(%d) of type (stream)

Pass some data between the sockets:
string(10) "0123456789"

Calling fsockopen() with address and port in same string:
resource(%d) of type (stream)

Pass some data between the sockets:
string(10) "0123456789"
Done
