--TEST--
Bug #61548 (content-type must appear at the end of headers)
--INI--
allow_url_fopen=1
--SKIPIF--
<?php require 'server.inc'; http_server_skipif('tcp://127.0.0.1:12342'); ?>
--FILE--
<?php
require 'server.inc';

function do_test($header) {
    $options = [
        'http' => [
			'method' => 'POST',
			'header' => $header,
            'follow_location' => true,
        ],
    ];

    $ctx = stream_context_create($options);

    $responses = [
		"data://text/plain,HTTP/1.1 201\r\nLocation: /foo\r\n\r\n",
		"data://text/plain,HTTP/1.1 200\r\nConnection: close\r\n\r\n",
	];
    $pid = http_server('tcp://127.0.0.1:12342', $responses, $output);

    $fd = fopen('http://127.0.0.1:12342/', 'rb', false, $ctx);
    fseek($output, 0, SEEK_SET);
    echo stream_get_contents($output);

    http_server_kill($pid);
}

do_test("First:1\nSecond:2\nContent-type: text/plain");
do_test("First:1\nSecond:2\nContent-type: text/plain\n");
do_test("First:1\nSecond:2\nContent-type: text/plain\nThird:");
do_test("First:1\nContent-type:text/plain\nSecond:2");
do_test("First:1\nContent-type:text/plain\nSecond:2\n");
do_test("First:1\nContent-type:text/plain\nSecond:2\nThird:");

?>
Done
--EXPECT--
POST / HTTP/1.0
Host: 127.0.0.1:12342
First:1
Second:2
Content-type: text/plain

GET /foo HTTP/1.0
Host: 127.0.0.1:12342
First:1
Second:2


POST / HTTP/1.0
Host: 127.0.0.1:12342
First:1
Second:2
Content-type: text/plain

GET /foo HTTP/1.0
Host: 127.0.0.1:12342
First:1
Second:2


POST / HTTP/1.0
Host: 127.0.0.1:12342
First:1
Second:2
Content-type: text/plain
Third:

GET /foo HTTP/1.0
Host: 127.0.0.1:12342
First:1
Second:2
Third:

POST / HTTP/1.0
Host: 127.0.0.1:12342
First:1
Content-type:text/plain
Second:2

GET /foo HTTP/1.0
Host: 127.0.0.1:12342
First:1
Second:2

POST / HTTP/1.0
Host: 127.0.0.1:12342
First:1
Content-type:text/plain
Second:2

GET /foo HTTP/1.0
Host: 127.0.0.1:12342
First:1
Second:2

POST / HTTP/1.0
Host: 127.0.0.1:12342
First:1
Content-type:text/plain
Second:2
Third:

GET /foo HTTP/1.0
Host: 127.0.0.1:12342
First:1
Second:2
Third:

Done

