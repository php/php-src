--TEST--
Bug #80256: file_get_contents strip first line with chunked encoding redirect
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require 'server.inc';

$responses = array(
    "data://text/plain,HTTP/1.1 302 Moved Temporarily\r\n"
    . "Location: /try-again\r\n"
    . "Transfer-Encoding: chunked\r\n\r\n"
    . "0\r\n\r\n",
    "data://text/plain,HTTP/1.1 200 Ok\r\n"
    . "Transfer-Encoding: chunked\r\n\r\n"
    . "4\r\n1234\r\n0\r\n\r\n",
);

['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

var_dump(file_get_contents($uri));

http_server_kill($pid);

?>
--EXPECT--
string(4) "1234"
