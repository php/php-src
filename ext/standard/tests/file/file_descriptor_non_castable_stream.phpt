--TEST--
Test file_descriptor() function: retrieve file descriptor on TCP socket
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
require __DIR__ . '/../http/server.inc'; http_server_skipif();
?>
--INI--
allow_url_fopen=1
--FILE--
<?php

require __DIR__ . '/../http/server.inc';

$responses = array(
    "data://text/plain,HTTP/1.0 200 Ok\r\nSome: Header\r\nSome: Header\r\n\r\nBody",
);

['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

/* Note: the warning is bogus in this case as no data actually gets lost,
 * but this checks that stream casting works */
$handle = fopen($uri, 'r');
$fd = file_descriptor($handle);
var_dump($fd);
var_dump(fread($handle, 20));
fclose($handle);

$socket = stream_socket_client('tcp://example.com:80', timeout: 0.5);
$fds = file_descriptor($socket);
var_dump($fds);
fclose($socket);

var_dump($fd == $fds);

http_server_kill($pid);
?>
--EXPECTF--
int(%d)
string(4) "Body"
int(%d)
bool(true)
