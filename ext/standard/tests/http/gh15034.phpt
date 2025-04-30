--TEST--
GH-15034 (Integer overflow on stream_notification_callback byte_max parameter with files bigger than 2GB)
--SKIPIF--
<?php
require 'server.inc';
http_server_skipif();
if (PHP_INT_SIZE != 8) die("skip 64-bit only");
?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require 'server.inc';

$responses = [
    "data://text/plain,HTTP/1.1 200 OK\r\n"
    . "Content-Type: text/plain\r\n"
    . "Content-Length: 3000000000\r\n\r\n"
    . "foo\n",
];
['pid' => $pid, 'uri' => $uri] = http_server($responses);

$params = ['notification' => function(
    int $notification_code,
    int $severity,
    ?string $message,
    int $message_code,
    int $bytes_transferred,
    int $bytes_max
) {
    global $max;
    $max = $bytes_max;
}];
$contextResource = stream_context_create([], $params);

$resource = fopen($uri, 'r', false, $contextResource);
fclose($resource);

http_server_kill($pid);

var_dump($max);
?>
--EXPECT--
int(3000000000)
