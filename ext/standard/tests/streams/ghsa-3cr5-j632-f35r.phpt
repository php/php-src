--TEST--
GHSA-3cr5-j632-f35r: Null byte termination in stream_socket_client() 
--FILE--
<?php

$server = stream_socket_server("tcp://localhost:0");
$socket_name = stream_socket_get_name($server, false);

if (preg_match('/:(\d+)$/', $socket_name, $m)) {
    $port = $m[1];
    $client = stream_socket_client("tcp://localhost\0.example.com:$port");
    var_dump($client);
    if ($client) {
        fclose($client);
    }
} else {
    echo "Could not extract port from socket name: $socket_name\n";
}

fclose($server);

?>
--EXPECTF--

Warning: stream_socket_client(): Unable to connect to tcp://localhost\0.example.com:%d (The hostname must not contain null bytes) in %s
bool(false)
