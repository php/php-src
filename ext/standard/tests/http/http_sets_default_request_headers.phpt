--TEST--
Test from and user-agent headers are set from ini and ctx
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--INI--
allow_url_fopen=1
from=ini_from@php.net
--FILE--
<?php
require 'server.inc';

$responses = array(
    "data://text/plain,HTTP/1.0 200 Ok\r\nContent-Length: 2\r\n\r\nok",
);

['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

$ctx = stream_context_create([
    'http' => [
        'user_agent' => 'SomeAgent',
        'content' => 'payload',
    ],
]);
file_get_contents($uri, false, $ctx);

http_server_kill($pid);

rewind($output);
$request = stream_get_contents($output);

var_dump(str_contains($request, 'User-Agent: SomeAgent'));
var_dump(str_contains($request, 'From: ini_from@php.net'));
var_dump(str_contains($request, 'Content-Type: application/x-www-form-urlencoded'));
var_dump(str_contains($request, 'Content-Length: 7'));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
