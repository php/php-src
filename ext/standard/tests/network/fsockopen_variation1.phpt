--TEST--
testing fsockopen without a protocol string
--FILE--
<?php

echo "Open a server socket\n";
$server = stream_socket_server('tcp://127.0.0.1:31337');

echo "\nCalling fsockopen() without a protocol in the hostname string:\n";
$hostname = '127.0.0.1';
$port = '31337';
$client = fsockopen($hostname, $port);
var_dump($client);
fclose($client);

echo "\nCalling fsockopen() with address and port in same string, without a protocol:\n";
$address = $hostname . ':' . $port;
$second_client = fsockopen($address);
var_dump($second_client);
fclose($second_client);

echo "Done";
?>
--EXPECTF--
Open a server socket

Calling fsockopen() without a protocol in the hostname string:
resource(%d) of type (stream)

Calling fsockopen() with address and port in same string, without a protocol:
resource(%d) of type (stream)
Done
