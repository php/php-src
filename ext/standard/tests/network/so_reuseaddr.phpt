--TEST--
stream_socket_server() SO_REUSEADDR context option test
--FILE--
<?php
$is_win = substr(PHP_OS, 0, 3) == "WIN";
// Test default behavior (SO_REUSEADDR enabled)
$server1 = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr, STREAM_SERVER_BIND | STREAM_SERVER_LISTEN);
if (!$server1) {
    die('Unable to create server3');
}

$addr = stream_socket_get_name($server1, false);
$port = (int)substr(strrchr($addr, ':'), 1);

$client1 = stream_socket_client("tcp://127.0.0.1:$port");
$accepted = stream_socket_accept($server1, 1);

// Force real TCP connection with data
fwrite($client1, "test");
fread($accepted, 4);
fwrite($accepted, "response");
fread($client1, 8);

fclose($client1);
if (!$is_win) { // Windows would always succeed if server is closed
    fclose($server1);
}

$server2 = @stream_socket_server("tcp://127.0.0.1:$port", $errno, $errstr, STREAM_SERVER_BIND | STREAM_SERVER_LISTEN);
if ($server2) {
    echo "Default: Server restart succeeded\n";
    fclose($server2);
} else {
    echo "Default: Server restart failed\n";
}

// Test with SO_REUSEADDR explicitly disabled
$context = stream_context_create(['socket' => ['so_reuseaddr' => false]]);
$server3 = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr, STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $context);
if (!$server3) {
    die('Unable to create server3');
}

$addr = stream_socket_get_name($server3, false);
$port = (int)substr(strrchr($addr, ':'), 1);

$client3 = stream_socket_client("tcp://127.0.0.1:$port");
$accepted = stream_socket_accept($server3, 1);

// Force real TCP connection with data
fwrite($client3, "test");
fread($accepted, 4);
fwrite($accepted, "response");
fread($client3, 8);

// Client closes first (becomes active closer)
fclose($client3);  // This enters TIME_WAIT
if (!$is_win) { // Windows would always succeed if server is closed
    fclose($server3);
}

// Try immediate bind with SO_REUSEADDR disabled (should fail)
$server4 = @stream_socket_server("tcp://127.0.0.1:$port", $errno, $errstr, STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $context);
if ($server4) {
    echo "Disabled: Server restart succeeded\n";
    fclose($server4);
} else {
    echo "Disabled: Server restart failed\n";
}
?>
--EXPECT--
Default: Server restart succeeded
Disabled: Server restart failed
