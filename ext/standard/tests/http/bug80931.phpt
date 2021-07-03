--TEST--
Bug #80931 (HTTP stream hangs if server doesn't close connection)
--INI--
allow_url_fopen=1
--SKIPIF--
<?php require 'server.inc'; http_server_skipif('tcp://127.0.0.1:12342'); ?>
--FILE--
<?php
require_once 'server.inc';

$responses = array(
    "data://text/plain,HTTP/1.1 200\r\nContent-Length: 10\r\n\r\n0123456789",
    "data://text/plain,HTTP/1.1 200\r\nTransfer-Encoding: chunked\r\n\r\n4\r\n****\r\n0\r\n\r\n",
    "data://text/plain,HTTP/1.1 200\r\nTransfer-Encoding: chunked\r\n\r\n4\r\n****\r\n0\r\n\r\n",
);
$pid = http_server("tcp://127.0.0.1:12342", $responses, $output);

$options = [
    'http'=> [
        'protocol_version' => "1.1",
        'header' => "Connection: keep-alive",
    ]
];

$stream = fopen("http://127.0.0.1:12342/", "r", false, stream_context_create($options));
$t0 = microtime(true);
var_dump(stream_get_contents($stream));
$t1 = microtime(true);
var_dump($t1 - $t0 < 2);

$stream = fopen("http://127.0.0.1:12342/", "r", false, stream_context_create($options));
$t0 = microtime(true);
var_dump(stream_get_contents($stream));
$t1 = microtime(true);
var_dump($t1 - $t0 < 2);

$options['http']['auto_decode'] = false;
$stream = fopen("http://127.0.0.1:12342/", "r", false, stream_context_create($options));
$t0 = microtime(true);
var_dump(stream_get_contents($stream));
$t1 = microtime(true);
var_dump($t1 - $t0 < 2);

http_server_kill($pid);
?>
--EXPECT--
string(10) "0123456789"
bool(true)
string(4) "****"
bool(true)
string(14) "4
****
0

"
bool(true)
