--TEST--
stream_isatty(): casting stream does not emit data loss and should not emit warnings
Bug GH-10092 (Internal stream casting should not emit lost bytes warning twice)
--SKIPIF--
<?php
require __DIR__ . '/../../ext/standard/tests/http/server.inc'; http_server_skipif();
?>
--INI--
allow_url_fopen=1
--FILE--
<?php

require __DIR__ . '/../../ext/standard/tests/http/server.inc';

$responses = array(
    "data://text/plain,HTTP/1.0 200 Ok\r\nSome: Header\r\nSome: Header\r\n\r\nBody",
);

['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

/* Note: the warning is bogus in this case as no data actually gets lost,
 * but this checks that stream casting works */
$handle = fopen($uri, 'r');
var_dump(stream_isatty($handle));
var_dump(fread($handle, 20));
fclose($handle);

http_server_kill($pid);
?>
--EXPECT--
bool(false)
string(4) "Body"
