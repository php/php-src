--TEST--
Stream notification callback cannot close the stream it is dispatched from
--SKIPIF--
<?php
require 'server.inc';
http_server_skipif();
?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require 'server.inc';

$server = http_server_init();
if (is_resource($server)) {
    /* Send the body only after the client write. */
    $conn = stream_socket_accept($server);
    while (($line = fgets($conn)) !== false && $line !== "\r\n");
    fwrite($conn, "HTTP/1.1 200 OK\r\nContent-Length: 4\r\nConnection: close\r\n\r\n");
    fread($conn, 2);
    fwrite($conn, "body");
    fclose($conn);
    exit(0);
}
['pid' => $pid, 'uri' => $uri] = $server;

$stream = null;
$context = stream_context_create([], ['notification' => function (int $code) use (&$stream) {
    if (!is_resource($stream)) {
        return;
    }
    if ($code === STREAM_NOTIFY_PROGRESS || $code === STREAM_NOTIFY_COMPLETED) {
        echo $code === STREAM_NOTIFY_PROGRESS ? "progress\n" : "completed\n";
        var_dump(fclose($stream));
    }
}]);

$stream = fopen($uri, 'r', false, $context);
var_dump(fwrite($stream, "go"));
var_dump(fread($stream, 8192));
var_dump(fread($stream, 8192));
var_dump(fclose($stream));

http_server_kill($pid);
?>
--EXPECTF--
progress

Warning: fclose(): %d is not a valid stream resource in %s on line %d
bool(false)
int(2)
progress

Warning: fclose(): %d is not a valid stream resource in %s on line %d
bool(false)
string(4) "body"
completed

Warning: fclose(): %d is not a valid stream resource in %s on line %d
bool(false)
string(0) ""
bool(true)
