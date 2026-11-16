--TEST--
GH-24508 (HTTP wrapper leaks the dechunk filter when header parsing fails after Transfer-Encoding)
--INI--
allow_url_fopen=1
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--FILE--
<?php
require 'server.inc';

['pid' => $pid, 'uri' => $uri] = http_server([
    "data://text/plain,HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\nBrokenHeaderNoColon\r\n\r\n",
]);

var_dump(@file_get_contents($uri));

http_server_kill($pid);
?>
--EXPECT--
bool(false)
