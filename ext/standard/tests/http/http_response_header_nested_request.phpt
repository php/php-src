--TEST--
http_get_last_response_headers() after a request made from a notification callback
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require 'server.inc';

$responses = [
    "data://text/plain,HTTP/1.0 200 Ok\r\nContent-Type: text/plain\r\nOuter: 1\r\n\r\nouter",
    "data://text/plain,HTTP/1.0 200 Ok\r\nInner: 1\r\n\r\ninner",
];

['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

$nested = false;
$ctx = stream_context_create([], ['notification' => function ($code) use (&$nested, $uri) {
    if ($code === STREAM_NOTIFY_MIME_TYPE_IS && !$nested) {
        $nested = true;
        var_dump(file_get_contents($uri));
    }
}]);

var_dump(file_get_contents($uri, false, $ctx));
var_dump(http_get_last_response_headers());

http_server_kill($pid);
?>
--EXPECT--
string(5) "inner"
string(5) "outer"
array(3) {
  [0]=>
  string(15) "HTTP/1.0 200 Ok"
  [1]=>
  string(24) "Content-Type: text/plain"
  [2]=>
  string(8) "Outer: 1"
}
