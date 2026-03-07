--TEST--
GH-20679 (finfo_file() doesn't work on remote resources)
--EXTENSIONS--
fileinfo
--INI--
allow_url_fopen=1
--SKIPIF--
<?php
if (@!include "./ext/standard/tests/http/server.inc") die('skip server.inc not available');
http_server_skipif();
?>
--FILE--
<?php
require "./ext/standard/tests/http/server.inc";

['pid' => $pid, 'uri' => $uri] = http_server([
    "data://text/plain,HTTP/1.0 200 Ok\r\n\r\n<html>foo",
], $output);

$f = finfo_open();
var_dump(finfo_file($f, $uri));

http_server_kill($pid);
?>
--EXPECT--
string(51) "HTML document, ASCII text, with no line terminators"
