--TEST--
GH-24509 (HTTP wrapper leaks the Location buffer when an empty Location header is followed by another)
--INI--
allow_url_fopen=1
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--FILE--
<?php
require 'server.inc';

['pid' => $pid, 'uri' => $uri] = http_server([
    "data://text/plain,HTTP/1.1 302 Found\r\nLocation:\r\nLocation: /abcdefghijklmno\r\nContent-Length: 0\r\n\r\n",
    "data://text/plain,HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK",
]);

var_dump(file_get_contents($uri));

http_server_kill($pid);
?>
--EXPECT--
string(2) "OK"
