--TEST--
Tests that stream_socket_listen() listens on a previously bound socket
--FILE--
<?php
$server = stream_socket_server('tcp://0.0.0.0:0', $errno, $errstr, STREAM_SERVER_BIND);
var_dump(stream_socket_listen($server));

$port = explode(':', stream_socket_get_name($server, FALSE), 2)[1];

$clientAddr = "tcp://127.0.0.1:{$port}";
$clientFlags = STREAM_CLIENT_CONNECT | STREAM_CLIENT_ASYNC_CONNECT;
$client = stream_socket_client($clientAddr, $errno, $errstr, $timeout = 0, $clientFlags);

while (TRUE) {
    $r = [$server];
    $w = $e = NULL;
    if (stream_select($r, $w, $e, $timeout = 1)) {
        $serverClient = stream_socket_accept($server, 1);
        break;
    }
}

fwrite($serverClient, "foo\n");
$sockData = trim(fgets($client));

var_dump($server, $client, $serverClient, $sockData);

fclose($server);
fclose($client);
fclose($serverClient);
?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
bool(true)
resource(%d) of type (stream)
resource(%d) of type (stream)
resource(%d) of type (stream)
string(3) "foo"
===DONE===
