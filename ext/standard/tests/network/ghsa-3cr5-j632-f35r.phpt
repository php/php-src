--TEST--
GHSA-3cr5-j632-f35r: Null byte termination in fsockopen() 
--FILE--
<?php

$server = stream_socket_server("tcp://localhost:0");

if (preg_match('/:(\d+)$/', stream_socket_get_name($server, false), $m)) {
    $client = fsockopen("localhost\0.example.com", intval($m[1]));
    var_dump($client);
    if ($client) {
        fclose($client);
    }
}
fclose($server);

?>
--EXPECTF--

Warning: fsockopen(): Unable to connect to localhost:%d (The hostname must not contain null bytes) in %s
bool(false)
