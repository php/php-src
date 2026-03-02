--TEST--
stream_socket_server() SO_REUSEPORT context option test
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == "WIN") {
    die('skip SO_REUSEPORT not available on Windows');
}
?>
--FILE--
<?php
// SO_REUSEPORT enabled - should allow multiple servers on same port
$context1 = stream_context_create(['socket' => ['so_reuseport' => true]]);
$server1 = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr, 
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $context1);

if (!$server1) {
    die('Unable to create server1');
}

$addr = stream_socket_get_name($server1, false);
$port = (int)substr(strrchr($addr, ':'), 1);

// Establish actual connection on server1
$client1 = stream_socket_client("tcp://127.0.0.1:$port");
$accepted1 = stream_socket_accept($server1, 1);

// Force real TCP connection with data
fwrite($client1, "test");
fread($accepted1, 4);
fwrite($accepted1, "response");
fread($client1, 8);

// Try to bind second server to SAME port with SO_REUSEPORT (while server1 still active)
$context2 = stream_context_create(['socket' => ['so_reuseport' => true]]);
$server2 = @stream_socket_server("tcp://127.0.0.1:$port", $errno, $errstr, 
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $context2);

if ($server2) {
    echo "SO_REUSEPORT enabled: Multiple servers succeeded\n";
    
    // Verify server2 can also accept connections
    $client2 = stream_socket_client("tcp://127.0.0.1:$port");
    $accepted2_on_srv1 = @stream_socket_accept($server1, 0.1);
    $accepted2_on_srv2 = @stream_socket_accept($server2, 0.1);
    
    if ($accepted2_on_srv1 || $accepted2_on_srv2) {
        echo "SO_REUSEPORT enabled: Connections accepted\n";
    }
    
    if ($accepted2_on_srv1) fclose($accepted2_on_srv1);
    if ($accepted2_on_srv2) fclose($accepted2_on_srv2);
    if ($client2) fclose($client2);
    
    fclose($server2);
} else {
    echo "SO_REUSEPORT enabled: Multiple servers failed\n";
}

fclose($accepted1);
fclose($client1);
fclose($server1);

// SO_REUSEPORT disabled (default) - should NOT allow multiple servers
$server3 = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr, 
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN);

if (!$server3) {
    die('Unable to create server3');
}

$addr = stream_socket_get_name($server3, false);
$port = (int)substr(strrchr($addr, ':'), 1);

$client3 = stream_socket_client("tcp://127.0.0.1:$port");
$accepted3 = stream_socket_accept($server3, 1);

fwrite($client3, "test");
fread($accepted3, 4);
fwrite($accepted3, "response");
fread($client3, 8);

// Try to bind second server WITHOUT SO_REUSEPORT (while server3 still active)
$server4 = @stream_socket_server("tcp://127.0.0.1:$port", $errno, $errstr, 
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN);

if ($server4) {
    echo "SO_REUSEPORT disabled: Multiple servers succeeded\n";
    fclose($server4);
} else {
    echo "SO_REUSEPORT disabled: Multiple servers failed\n";
}

fclose($accepted3);
fclose($client3);
fclose($server3);

// SO_REUSEPORT explicitly disabled
$context3 = stream_context_create(['socket' => ['so_reuseport' => false]]);
$server5 = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr, 
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $context3);

if (!$server5) {
    die('Unable to create server5');
}

$addr = stream_socket_get_name($server5, false);
$port = (int)substr(strrchr($addr, ':'), 1);

$client5 = stream_socket_client("tcp://127.0.0.1:$port");
$accepted5 = stream_socket_accept($server5, 1);

fwrite($client5, "test");
fread($accepted5, 4);
fwrite($accepted5, "response");
fread($client5, 8);

$context4 = stream_context_create(['socket' => ['so_reuseport' => false]]);
$server6 = @stream_socket_server("tcp://127.0.0.1:$port", $errno, $errstr, 
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $context4);

if ($server6) {
    echo "SO_REUSEPORT explicitly disabled: Multiple servers succeeded\n";
    fclose($server6);
} else {
    echo "SO_REUSEPORT explicitly disabled: Multiple servers failed\n";
}

fclose($accepted5);
fclose($client5);
fclose($server5);
?>
--EXPECT--
SO_REUSEPORT enabled: Multiple servers succeeded
SO_REUSEPORT enabled: Connections accepted
SO_REUSEPORT disabled: Multiple servers failed
SO_REUSEPORT explicitly disabled: Multiple servers failed
