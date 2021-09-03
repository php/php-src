--TEST--
Bug #65634 (HTTP wrapper is very slow with protocol_version 1.1)
--INI--
allow_url_fopen=1
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--FILE--
<?php
require 'server.inc';

function do_test($version, $connection) {
    $options = [
        'http' => [
            'protocol_version' => $version,
        ],
    ];

    if ($connection) {
        $options['http']['header'] = "Connection: $connection";
    }

    $ctx = stream_context_create($options);

    $responses = ["data://text/plain,HTTP/$version 204 No Content\r\n\r\n"];
    ['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

    $fd = fopen($uri, 'rb', false, $ctx);
    fseek($output, 0, SEEK_SET);
    echo stream_get_contents($output);

    http_server_kill($pid);
}

echo "HTTP/1.0, default behaviour:\n";
do_test('1.0', null);

echo "HTTP/1.0, connection: close:\n";
do_test('1.0', 'close');

echo "HTTP/1.0, connection: keep-alive:\n";
do_test('1.0', 'keep-alive');

echo "HTTP/1.1, default behaviour:\n";
do_test('1.1', null);

echo "HTTP/1.1, connection: close:\n";
do_test('1.1', 'close');

echo "HTTP/1.1, connection: keep-alive:\n";
do_test('1.1', 'keep-alive');

--EXPECTF--
HTTP/1.0, default behaviour:
GET / HTTP/1.0
Host: %s:%d
Connection: close

HTTP/1.0, connection: close:
GET / HTTP/1.0
Host: %s:%d
Connection: close

HTTP/1.0, connection: keep-alive:
GET / HTTP/1.0
Host: %s:%d
Connection: keep-alive

HTTP/1.1, default behaviour:
GET / HTTP/1.1
Host: %s:%d
Connection: close

HTTP/1.1, connection: close:
GET / HTTP/1.1
Host: %s:%d
Connection: close

HTTP/1.1, connection: keep-alive:
GET / HTTP/1.1
Host: %s:%d
Connection: keep-alive
