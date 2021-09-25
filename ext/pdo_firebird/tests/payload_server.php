<?php

$socket = stream_socket_server("tcp://localhost:0", $errno, $errstr);
if (!$socket) {
    echo "Can't start server: $errstr ($errno)\n";
    exit(1);
}

$filename = $argv[1];
$payload = file_get_contents($filename);
if ($payload === false) {
    echo "Can't read $filename\n";
    exit(1);
}

fputs(STDERR, "FB payload server listening on " . stream_socket_get_name($socket, false) . "\n");

while ($conn = stream_socket_accept($socket)) {
    fwrite($conn, $payload);
}
